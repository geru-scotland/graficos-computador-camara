#include "headers/shared_defines.h"
#include "headers/functions.h"

/***********************************************************************
 *                                                                     *
 *                          TRANSFORMACIONES                           *
 *                                                                     *
 ***********************************************************************/

/***********************************************************************
 * Este archivo implementa las funciones de transformación para objetos
 * y cámaras. Incluye rotaciones, traslaciones
 * y escalados, aplicables tanto a objetos individuales como a la cámara,
 * en sus diferentes modos (análisis, vuelo...) y ejes.
 ***********************************************************************/

/**
 * Inicializa una matriz de rotación en base a un eje específico (X, Y, Z) y
 * un ángulo theta. La matriz resultante puede ser utilizada para rotar
 * objetos o cámaras alrededor del eje especificado.
 * @param eje Caracter que especifica el eje de rotación ('x', 'y', 'z').
 * @param theta Ángulo de rotación en radianes.
 * @param matriz_rotacion Matriz de rotación resultante.
 */
void set_rotation_matrix(char eje, float theta, double matriz_rotacion[4][4]) {
    // Init matriz de rotación como matriz identidad
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            matriz_rotacion[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }

    float cosTheta = cos(theta);
    float sinTheta = sin(theta);

    // Matriz de rotación según eje
    switch (eje) {
        case 'x':
        case 'X':
            matriz_rotacion[1][1] = cosTheta;
            matriz_rotacion[1][2] = -sinTheta;
            matriz_rotacion[2][1] = sinTheta;
            matriz_rotacion[2][2] = cosTheta;
            break;
        case 'y':
        case 'Y':
            matriz_rotacion[0][0] = cosTheta;
            matriz_rotacion[0][2] = sinTheta;
            matriz_rotacion[2][0] = -sinTheta;
            matriz_rotacion[2][2] = cosTheta;
            break;
        case 'z':
        case 'Z':
            matriz_rotacion[0][0] = cosTheta;
            matriz_rotacion[0][1] = -sinTheta;
            matriz_rotacion[1][0] = sinTheta;
            matriz_rotacion[1][1] = cosTheta;
            break;
        default:
            printf("Eje no válido\n");
            break;
    }
}

/**
 * Rota un objeto o cámara alrededor de un eje específico. La dirección y
 * magnitud de la rotación son definidas por parámetros.
 * @param eje Eje de rotación.
 * @param dir Dirección y magnitud de la rotación.
 * @param camera Puntero a la cámara (si aplicable).
 * @param scene_status_mask Máscara de estado de la escena.
 * @param sel_ptr Puntero al objeto seleccionado para la rotación.
 */
void rotate(char eje, int dir, Camera* camera, unsigned int scene_status_mask, triobj* sel_ptr) {
    float angulo = scene_status_mask & MODO_CAMARA ? ROTACION_ANGULO_CAMARA : ROTACION_ANGULO;
    float theta = angulo * (PI / 180.0) * dir; // Rotación de 2 grados

    double matriz_rotacion[4][4];

    set_rotation_matrix(eje, theta, matriz_rotacion);

    float temp[4][4] = {{0}};

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 4; ++k) {
                // Si eje local, la matriz de rotación se multiplica a la derecha de la matriz de transformación acumulada
                // T * R * punto
                if(scene_status_mask & EJE_LOCAL)
                {
                    // En el caso de estar en el modo cámara, que rote sobre sus ejes locales
                    // Solo si está en modo vuelo.
                    if(scene_status_mask & MODO_CAMARA && !(scene_status_mask & CAMARA_ANALISIS))
                        temp[i][j] += matriz_rotacion[i][k] * camera->view->matrix[k][j];
                    else
                        temp[i][j] += sel_ptr->mptr->m[i*4 + k] * matriz_rotacion[k][j];
                }
                else
                {
                    // Si eje global, la matriz de rotación se multiplica a la izquierda de la matriz de transformación acumulada
                    // R * T * punto
                    if(scene_status_mask & MODO_CAMARA)
                        temp[i][j] += matriz_rotacion[i][k] * camera->view->matrix[k][j];
                    else
                        temp[i][j] += matriz_rotacion[i][k] * sel_ptr->mptr->m[k*4 + j];
                }
            }
        }
    }

    // Copio la matriz temporal de vuelta en la matriz acumulada
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if(scene_status_mask & MODO_CAMARA)
                camera->view->matrix[i][j] = temp[i][j];
            else{
                sel_ptr->mptr->m[i*4 + j] = temp[i][j];
                if(scene_status_mask & MODO_OBJETO)
                    camera->view->matrix[i][j] = temp[i][j];
            }
        }
    }

    // Matriz vista de cámara actualizada, ahora updatear los vectores.
    update_camera_vectors_from_view_matrix(camera);
}

/**
 * Aplica una traslación local a un objeto o cámara, moviéndolos en el
 * espacio según un eje y dirección dados.
 * @param eje Eje de traslación.
 * @param dir Dirección y magnitud de la traslación.
 * @param camera Puntero a la cámara (si aplicable).
 * @param scene_status_mask Máscara de estado de la escena.
 * @param sel_ptr Puntero al objeto seleccionado para la traslación.
 */
void traslacion_local(char eje, int dir, Camera* camera, unsigned int scene_status_mask, triobj* sel_ptr){
    Vector3Index vector_eje;

    // Verificar primero si el modo cámara está activo
    if (!(scene_status_mask & MODO_CAMARA)) {
        // Si el modo cámara NO está activo, el objeto será el que se tenga
        // que trasladar sobre sus ejes locales.
        // Determinar el índice basado en el eje proporcionado
        switch (eje) {
            case 'x':
            case 'X':
                vector_eje.x = 0;
                vector_eje.y = 4;
                vector_eje.z = 8;
                break;
            case 'y':
            case 'Y':
                vector_eje.x = 1;
                vector_eje.y = 5;
                vector_eje.z = 9;
                break;
            case 'z':
            case 'Z':
                vector_eje.x = 2;
                vector_eje.y = 6;
                vector_eje.z = 10;
                break;
            default:
                return; // Eje inválido
        }

        Vector3 vector_direccion;
        vector_direccion.x = sel_ptr->mptr->m[vector_eje.x];
        vector_direccion.y = sel_ptr->mptr->m[vector_eje.y];
        vector_direccion.z = sel_ptr->mptr->m[vector_eje.z];

        // Normalizar el vector de dirección
        vector_direccion = normalizar_vector(vector_direccion);

        // Ahora ya aplico la traslación al objeto
        sel_ptr->mptr->m[3] += TRASLACION_PIXELS * vector_direccion.x * dir;
        sel_ptr->mptr->m[7] += TRASLACION_PIXELS * vector_direccion.y * dir;
        sel_ptr->mptr->m[11] += TRASLACION_PIXELS * vector_direccion.z * dir;

    } else {
        // Si el modo cámara está activo.
        // Solo continuar si está el modo VUELO
        // o si Análisis y eje Z.
        if(scene_status_mask & CAMARA_ANALISIS && (eje != 'z' && eje != 'Z'))
            return;

        switch (eje) {
            case 'x':
            case 'X': // Yaw
                if(!(scene_status_mask & EJE_LOCAL)){
                    scene_status_mask |= EJE_LOCAL;
                }
                rotate('y', dir, camera, scene_status_mask, sel_ptr);
                break;
            case 'y':
            case 'Y': // Pitch
                if(!(scene_status_mask & EJE_LOCAL)){
                    scene_status_mask |= EJE_LOCAL;
                }
                rotate('x', dir, camera, scene_status_mask, sel_ptr);
                break;
            case 'z':
            case 'Z':
            {
                // Si Z, hacia delante o hacia atrás.
                // Si X o Y, rotar y cambiar dirección de Z.
                Vector3 vector_direccion_camara;
                vector_direccion_camara.x = camera->view->matrix[0][2];
                vector_direccion_camara.y = camera->view->matrix[1][2];
                vector_direccion_camara.z = camera->view->matrix[2][2];

                // Normalizo el vector de dirección de la cámara ya que la matriz está normalizada
                vector_direccion_camara = normalizar_vector(vector_direccion_camara);

                // Aplico la traslación a la cámara
                camera->view->matrix[0][3] += TRASLACION_PIXELS * vector_direccion_camara.x * dir * (-1);
                camera->view->matrix[1][3] += TRASLACION_PIXELS * vector_direccion_camara.y * dir * (-1);
                camera->view->matrix[2][3] += TRASLACION_PIXELS * vector_direccion_camara.z * dir * (-1);
                break;
            }
            default:
                return;
        }
    }
}

/**
 * Cámara en modo análisis. Realiza una traslación en "órbita" alrededor
 * de un objeto seleccionado.
 * @param eje Eje de rotación para la órbita.
 * @param dir Dirección y magnitud de la órbita.
 * @param camera Puntero a la cámara.
 * @param sel_ptr Puntero al objeto alrededor del cual se orbita.
 */
void traslacion_orbita(char eje, int dir, Camera* camera, triobj* sel_ptr){
    // 1) Restar vector vector posicon camara - centro_objeto (al revés, en OpenGL)
    // 2) Trasladar con esa diferencia
    // 3) Rotar como se desee, en base al eje
    // 4) Trasladar de vuelta, inversa (simplemente negar la traslación inicial)
    Vector3 obj_position_vector, vector_traslacion;
    Punto obj_position_point;

    float theta = ROTACION_ANGULO_CAMARA * (PI / 180.0) * dir; // Rotación de 2 grados

    double matriz_traslacion[4][4] = {{0}};
    double matriz_rotacion[4][4] = {{0}};
    double matriz_traslacion_inversa[4][4] = {{0}};
    double matriz_resultante[4][4] = {{0}};


    mxp(&obj_position_point, sel_ptr->mptr->m, calcular_centroide(sel_ptr));

    point_to_vector(obj_position_point, &obj_position_vector);

    vector_traslacion = vector3_substract(obj_position_vector, camera->eye_position);

    // Ahora monto la matriz traslación
    matriz_traslacion[0][0] = matriz_traslacion[1][1] = matriz_traslacion[2][2] = matriz_traslacion[3][3] = 1;
    matriz_traslacion[0][3] = vector_traslacion.x;
    matriz_traslacion[1][3] = vector_traslacion.y;
    matriz_traslacion[2][3] = vector_traslacion.z;

    // Invertir la matriz de traslación para la operación de deshacer, volver
    matriz_traslacion_inversa[0][0] = matriz_traslacion_inversa[1][1] = matriz_traslacion_inversa[2][2] = matriz_traslacion_inversa[3][3] = 1;
    matriz_traslacion_inversa[0][3] = -vector_traslacion.x;
    matriz_traslacion_inversa[1][3] = -vector_traslacion.y;
    matriz_traslacion_inversa[2][3] = -vector_traslacion.z;

    // Configurar matriz de rotación
    set_rotation_matrix(eje, theta, matriz_rotacion);

    // Multiplicar matrices de traslación y rotación
    // Ojo, orden de operaciones inverso a lectura de fórmula
    // Quiero que primero se haga la traslación inversa (en negtivo)
    // Luego se rote, y luego se deshaga la traslación.
    // Se hace este orden para compensar el eje z, que en OpenGL
    // Forward o hacia el frente, está en -z.
    matrix_multiplication(matriz_traslacion_inversa, matriz_rotacion, matriz_resultante);

    // Multiplicar la matriz resultante por la matriz de traslación inversa
    matrix_multiplication(matriz_resultante, matriz_traslacion, matriz_resultante);

    // Y ahora ya,  el resultado por la matriz de vista de la cámara
    double matriz_vista_actualizada[4][4] = {{0}};
    matrix_multiplication(matriz_resultante, camera->view->matrix, matriz_vista_actualizada);

    // Actualizo la matriz de vista de la cámara
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            camera->view->matrix[i][j] = matriz_vista_actualizada[i][j];
        }
    }

    update_camera_vectors(camera, obj_position_vector);
}


/**
 * Realiza una transformación general (traslación, rotación, escalado) sobre
 * un objeto o cámara basándose en el eje, dirección y modo de transformación.
 *
 * Dejo estos datos que me han sido de utilidad durante el desarrollo:
 *
 * Representación de una matriz de transformación 4x4:
 *
 * | r00 r01 r02 tx |
 * | r10 r11 r12 ty |
 * | r20 r21 r22 tz |
 * | 0   0   0   1  |
 *
 * Donde:
 * - r00, r01, r02, r10, r11, r12, r20, r21 y r22 representan la matriz de rotación.
 *   Estos valores cambiarán dependiendo de los ángulos de rotación alrededor de los ejes x, y, z.
 *
 * - tx, ty y tz representan la traslación en los ejes x, y y z respectivamente.
 *
 * - La última fila (0, 0, 0, 1) es estándar para matrices de transformación afines en gráficos 3D.
 *
 * @param eje Eje de transformación.
 * @param dir Dirección y magnitud de la transformación.
 * @param camera Puntero a la cámara (si aplicable).
 * @param scene_status_mask Máscara de estado de la escena que define el modo de transformación.
 * @param sel_ptr Puntero al objeto seleccionado para transformar.
 */
void transformar(char eje, int dir, Camera* camera, unsigned int scene_status_mask, triobj* sel_ptr) {

    if (gestionar_nueva_matriz(sel_ptr)!= NULL) {
        // Determinar la dirección basada en el eje actual
        int ejeNegativo_flag;
        int traslacionIndex;

        switch (eje) {
            case 'x':
                ejeNegativo_flag = EJE_X_NEGATIVO;
                traslacionIndex = 3;
                break;
            case 'y':
                ejeNegativo_flag = EJE_Y_NEGATIVO;
                traslacionIndex = 7;
                break;
            case 'z':
                ejeNegativo_flag = EJE_Z_NEGATIVO;
                traslacionIndex = 11;
                break;
            default:
                return;
        }

        if (scene_status_mask & MODO_TRASLACION) {
            if(scene_status_mask & EJE_LOCAL)
                traslacion_local(eje, dir, camera, scene_status_mask, sel_ptr);
            else if(scene_status_mask & CAMARA_ANALISIS)
                if(eje != 'z')
                    traslacion_orbita(eje, dir, camera, sel_ptr);
                else
                    traslacion_local(eje, dir, camera, scene_status_mask, sel_ptr);
            else
            {
                if(scene_status_mask & MODO_CAMARA)
                    (&camera->view->matrix[0][0])[traslacionIndex] += TRASLACION_PIXELS * dir * (-1); // -1 - Cámara al revés
                else {
                    sel_ptr->mptr->m[traslacionIndex] += TRASLACION_PIXELS * dir;
                    // A parte, si estamos en modo Camara objeto.
                    // Transformar también su posición
                    // Creo que no termina de persistir, mirar esto bien.
                    // Igual algo del camera swap.
                    if(scene_status_mask & MODO_OBJETO)
                        (&camera->view->matrix[0][0])[traslacionIndex] += TRASLACION_PIXELS * dir * (-1); // -1 Cámara al revés
                }
            }
        }
        else if (scene_status_mask & MODO_ROTACION) {
            rotate(eje, dir, camera, scene_status_mask, sel_ptr);
        }
        else if (scene_status_mask & MODO_ESCALADO) {
            for (int i = 0; i < 16; i++) {
                if (dir > 0) {
                    sel_ptr->mptr->m[i] *= ESCALADO_ESCALA;
                } else {
                    sel_ptr->mptr->m[i] /= ESCALADO_ESCALA;
                }
            }
        }
    }
}