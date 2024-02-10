#include "headers/shared_defines.h"
#include "headers/functions.h"

/***********************************************************************
 *                                                                     *
 *                              CÁMARA                                 *
 *                                                                     *
 ***********************************************************************/

/***********************************************************************
 * Este archivo contiene la implementación de las funcionalidades de la
 * cámara de la aplicación. Incluye la definición de
 * constantes de proyección, funciones para establecer y actualizar
 * matrices de vista y proyección, y funciones para gestionar el
 * comportamiento de la cámara en relación con la escena 3D, como actualizar
 * sus vectores, gestión de cambio a modo objeto ("swap")..etc.
 ***********************************************************************/

const ProjectionConst ProjectionData = {
    .near_plane = 1.0f,
    .far_plane = 500.0f,
    .left = -1.0f,
    .right = 1.0f,
    .bottom = -1.0f,
    .top = 1.0f
};

/**
 * Establece la matriz de vista de la cámara. Esta matriz transforma
 * coordenadas del mundo a coordenadas de la cámara.
 * Ojo, se nutre de los vectores de la estructura Camera.
 * @param cam Puntero a la estructura de la cámara para establecer su matriz de vista.
 */
void set_view_matrix(Camera* cam) {
    // Inicializo la matriz a toda la matriz a cero
    // Todo: por consistencia, pon esto como estaba antes
    // Inicialización con {}
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            cam->view->matrix[i][j] = 0.0f;
        }
    }

    // Ahora, establecemos los valores específicos de la matriz de vista
    /*
        | Rx  Ux  -Fx  Tx |
        | Ry  Uy  -Fy  Ty |
        | Rz  Uz  -Fz  Tz |
        | 0   0    0   1  |
     */
    cam->view->matrix[0][0] = cam->vector_right.x;
    cam->view->matrix[1][0] = cam->vector_right.y;
    cam->view->matrix[2][0] = cam->vector_right.z;

    cam->view->matrix[0][1] = cam->vector_up.x;
    cam->view->matrix[1][1] = cam->vector_up.y;
    cam->view->matrix[2][1] = cam->vector_up.z;

    cam->view->matrix[0][2] = -cam->vector_forward.x;
    cam->view->matrix[1][2] = -cam->vector_forward.y;
    cam->view->matrix[2][2] = -cam->vector_forward.z;

    /**
     * La traslación en la matriz de vista "deshace" la posición de la cámara en el mundo.
     * Al aplicar esta matriz a los objetos de la escena, se traslada el mundo en la dirección opuesta
     * a la posición inicial de la cámara, colocando a la cámara en el origen de su propio sistema
     * de coordenadas.
     */
    cam->view->matrix[0][3] = -vector3_dot_product(cam->vector_right, cam->eye_position);
    cam->view->matrix[1][3] = -vector3_dot_product(cam->vector_up, cam->eye_position);
    cam->view->matrix[2][3] = -vector3_dot_product(cam->vector_forward, cam->eye_position);

    cam->view->matrix[3][3] = 1.0f;
}

/**
 * Copia los parámetros y la matriz de vista de una cámara a otra.
 * Útil para el cambio a cámara objeto o swap.
 * @param src Puntero a la cámara fuente.
 * @param dest Puntero a la cámara destino.
 */
void copy_camera(Camera* src, Camera* dest) {
    // Copio los valores de los vectores directamente.
    dest->eye_position = src->eye_position;
    dest->look_at = src->look_at;
    dest->vector_forward = src->vector_forward;
    dest->vector_right = src->vector_right;
    dest->vector_up = src->vector_up;

    // Matriz View, la copio.
    memcpy(dest->view->matrix, src->view->matrix, sizeof(src->view->matrix));
}

/**
 * Actualiza los parámetros y vectores de la cámara basados en una nueva posición,
 * punto de mira y vector up.
 * @param cam Puntero a la cámara a actualizar.
 * @param eye_position Nueva posición de la cámara.
 * @param look_at Punto hacia el que mira la cámara.
 * @param up_vector Vector que define la orientación vertical de la cámara.
 */
void update_camera(Camera* cam, Vector3 eye_position, Vector3 look_at, Vector3 up_vector){
    cam->eye_position = eye_position;
    cam->look_at = look_at;
    // Vectores
    cam->vector_up = normalizar_vector(up_vector);
    /* Ojo, se usa eye_position - look_at para alinear el vector_forward
     * de la cámara con la convención de mirar hacia el -Z en su propio sistema de referencia.
     * Ya que, lo que está "hacia el frente" tendrá siempre -z, y para simular esto, necesitamos
     * producir el vector contrario (lo lógico sería de la cámara al look at, pero ahora lo invertimos.
     * */
    cam->vector_forward = normalizar_vector(vector3_substract(cam->look_at, cam->eye_position));

    // Ahora, vector right - perpendicular a los vectores up y forward
    // Ojo, acuerdate de la regla de la mano derecha aqui, para el cross product
    // "Primero el pulgar"
    cam->vector_right = normalizar_vector(vector3_cross_product(cam->vector_up, cam->vector_forward));

    set_view_matrix(cam);
}

/**
 * Establece la matriz de proyección en perspectiva para la cámara.
 * @param pm Matriz de proyección a establecer.
 * @param near, far, right, left, top, bottom Parámetros del frustum de proyección.
 */
void set_perspective_projection_matrix(double pm[4][4], double near, double far, double right, double left, double top, double bottom) {
    pm[0][0] = 2 * near / (right - left);
    pm[0][1] = 0;
    pm[0][2] = (right + left) / (right - left);
    pm[0][3] = 0;

    pm[1][0] = 0;
    pm[1][1] = 2 * near / (top - bottom);
    pm[1][2] = (top + bottom) / (top - bottom);
    pm[1][3] = 0;

    pm[2][0] = 0;
    pm[2][1] = 0;
    pm[2][2] = -(far + near) / (far - near);
    pm[2][3] = -2 * far * near / (far - near);

    pm[3][0] = 0;
    pm[3][1] = 0;
    pm[3][2] = -1;
    pm[3][3] = 0;
}

/**
 * Establece la matriz de proyección ortográfica para la cámara.
 * @param pm Matriz de proyección a establecer.
 * @param near, far, right, left, top, bottom Parámetros del volumen de proyección.
 */
void set_orthographic_projection_matrix(double pm[4][4], double near, double far, double right, double left, double top, double bottom) {
    memset(pm, 0, sizeof(double) * 4 * 4);

    // Valores matriz ortografica, sin dividir
    pm[0][0] = 2 / (right - left);
    pm[0][3] = -(right + left) / (right - left);

    pm[1][1] = 2 / (top - bottom);
    pm[1][3] = -(top + bottom) / (top - bottom);

    pm[2][2] = -2 / (far - near);
    pm[2][3] = -(far + near) / (far - near);

    pm[3][3] = 1;
}

/**
 * Procesa un triángulo a través de la pipeline de la cámara, aplicando
 * transformaciones de modelo, vista y proyección.
 * @param main_camera Puntero a la cámara principal.
 * @param scene_mask Máscara de configuración de la escena.
 * @param triangulo_procesado Puntero al triángulo resultante.
 * @param triangulo Puntero al triángulo original.
 * @param matriz_transformacion Matriz de transformación del modelo.
 */
void camera_pipeline(Camera* main_camera, unsigned int scene_mask, Triangulo* triangulo_procesado, Triangulo* triangulo, double matriz_transformacion[16]){

    /**
     * Etapas de la Pipeline:
     * 1) Transformación del Modelo: Aplicar transformaciones (escalar, rotar, trasladar) a los vértices del modelo.
     * 2) Transformación de Vista: Aplicar la matriz de vista de la cámara para transformar la escena a la perspectiva de la cámara.
     * 3) Proyección: Proyectar la escena 3D en un plano 2D para su visualización.
     *    Nota: Este paso se aplicará después de la transformación de vista en la pipeline.
     *
     * La pipeline transforma el modelo desde sus coordenadas locales a coordenadas del mundo, y luego
     * al sistema de coordenadas de la cámara usando la matriz de vista.
     */

    // Ale, goazen.

    // 1) Transformación del modelo
    mxp(&triangulo_procesado->p1, matriz_transformacion, triangulo->p1);
    mxp(&triangulo_procesado->p2, matriz_transformacion, triangulo->p2);
    mxp(&triangulo_procesado->p3, matriz_transformacion, triangulo->p3);

    // 2) Transformación de vista
    mxp(&triangulo_procesado->p1, &main_camera->view->matrix[0][0], triangulo_procesado->p1);
    mxp(&triangulo_procesado->p2, &main_camera->view->matrix[0][0], triangulo_procesado->p2);
    mxp(&triangulo_procesado->p3, &main_camera->view->matrix[0][0], triangulo_procesado->p3);

    // 3) Proyección
    double projection_matrix[4][4];

    if (scene_mask & PROJECTION_PERSPECTIVE) {
        // Configuración para proyección en perspectiva
        set_perspective_projection_matrix(projection_matrix, ProjectionData.near_plane, ProjectionData.far_plane, ProjectionData.right, ProjectionData.left, ProjectionData.top, ProjectionData.bottom);
    } else {
        // Configuración para proyección ortográfica
        set_orthographic_projection_matrix(projection_matrix, ProjectionData.near_plane, ProjectionData.far_plane, ProjectionData.right, ProjectionData.left, ProjectionData.top, ProjectionData.bottom);
    }

    Punto punto1, punto2, punto3;
    mxp(&punto1, &projection_matrix[0][0], triangulo_procesado->p1);
    mxp(&punto2, &projection_matrix[0][0], triangulo_procesado->p2);
    mxp(&punto3, &projection_matrix[0][0], triangulo_procesado->p3);

    if(scene_mask & PROJECTION_PERSPECTIVE) {
        // Almacenar temporalmente los valores proyectados antes de la división por la profundidad
        scale_point(&punto1, PERSPECTIVE_FACTOR);
        scale_point(&punto2, PERSPECTIVE_FACTOR);
        scale_point(&punto3, PERSPECTIVE_FACTOR);

        apply_perspective_depth(triangulo_procesado, &punto1, &punto2, &punto3);
    }
}

/**
 * Intercambia entre la cámara principal y una cámara secundaria, ajustando
 * su configuración basada en el estado de la escena (mediante bitmask).
 * @param scene_status_mask Máscara de estado de la escena.
 * @param sel_ptr Puntero al objeto seleccionado en la escena.
 * @param main_camera Puntero a la cámara principal.
 * @param secondary_camera Puntero a la cámara secundaria.
 */
void swap_camera(unsigned int scene_status_mask, triobj* sel_ptr, Camera* main_camera, Camera* secondary_camera){

     if(scene_status_mask & MODO_OBJETO){
         /**
          * 1) Hallo el baricentro o centroide del objeto actual, para poder obtener
          *    el EYE_POS
          * 2) init_camera, lo tiene que hacer por parámetro y no simple acceso a var global.
          *   crear una cámara secundaria, asignarle datos y que la view sea modificada con estos.
          *   Previamente, haber guardado los datos de la cámara actual y colocarlos en la secondary
          *   El swap tiene que ser un baile de matrices.
          *   Establece primero camera->eye_position, camera->look_at etc,
          * 3)
          * 4)
          */
          Vector3 eye_vector, lookat_vector, up_vector;
          Punto eye_point, lookat_point;
          // Al centroide, le multiplico las transformaciones que haya tenido el objeto y ese
          // será el eye position del objeto. Necesito saber la nueva posición del centroide, claro.
          mxp(&eye_point, sel_ptr->mptr->m, calcular_centroide(sel_ptr));
          point_to_vector(eye_point, &eye_vector);
          // Look at, centroide -z? Restar algún valor en z. Si no mirar a ver cómo calcular orientació.
          lookat_point.x = -sel_ptr->mptr->m[2];
          lookat_point.y = -sel_ptr->mptr->m[6];
          lookat_point.z = -sel_ptr->mptr->m[10];
          point_to_vector(lookat_point, &lookat_vector);
          up_vector = vector3(0.0f, 1.0f, 0.0f);
          // Guardar datos de camara actual en cámara secundaria
          copy_camera(main_camera, secondary_camera);
          // Updatear camara principal con los nuevos valores
          update_camera(main_camera, eye_vector, normalizar_vector(lookat_vector), up_vector);
     } else {
         // Restarurar cámara secundaria -> cámara principal.
         View view;
         Camera aux;
         aux.view = &view;
         copy_camera(main_camera, &aux);
         copy_camera(secondary_camera, main_camera);
         copy_camera(&aux, secondary_camera);
     }
}


/**
 * Actualiza la posición de la cámara aplicando una matriz de transformación.
 * @param main_camera Puntero a la cámara a actualizar.
 * @param matriz_resultante Matriz de transformación a aplicar.
 */
void update_camera_position(Camera* main_camera) {
    main_camera->eye_position.x = main_camera->view->matrix[0][3];
    main_camera->eye_position.y = main_camera->view->matrix[1][3];
    main_camera->eye_position.z = main_camera->view->matrix[2][3];
}

/**
 * Actualiza los vectores de dirección de una cámara basándose en un nuevo punto de mira o focus.
 * @param camera Puntero a la cámara a actualizar.
 * @param look_at Nuevo punto de mira o focus.
 */
void update_camera_vectors(Camera* camera, Vector3 look_at) {
    if (camera == NULL) {
        return;
    }

    Vector3 direction_to_look_at = vector3_substract(look_at, camera->eye_position);

    // Eye position, camara posición es la 4ª columna de view matrix.
    camera->vector_forward = normalizar_vector(direction_to_look_at);
    camera->vector_right = normalizar_vector(vector3_cross_product(camera->vector_up, camera->vector_forward));
    camera->vector_up = normalizar_vector(vector3_cross_product(camera->vector_forward, camera->vector_right));
}

/**
 * Actualiza los vectores de la cámara basándose en su matriz de vista actual.
 * @param camera Puntero a la cámara a actualizar.
 */
void update_camera_vectors_from_view_matrix(Camera* camera) {
    if (camera == NULL) {
        return;
    }

    // Extrayendo el vector forward de la matriz de vista
    camera->vector_forward.x = camera->view->matrix[0][2];
    camera->vector_forward.y = camera->view->matrix[1][2];
    camera->vector_forward.z = camera->view->matrix[2][2];

    camera->vector_forward = normalizar_vector(camera->vector_forward);

    camera->vector_right = normalizar_vector(vector3_cross_product(camera->vector_up, camera->vector_forward));
    camera->vector_up = normalizar_vector(vector3_cross_product(camera->vector_forward, camera->vector_right));
}