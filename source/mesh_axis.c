#include "headers/shared_defines.h"
#include "headers/functions.h"

/***********************************************************************
 *                                                                     *
 *                              MALLAS Y EJES                          *
 *                                                                     *
 ***********************************************************************/

/**
 * Dibuja los ejes coordenados en el espacio de visualización.
 * El eje X se dibuja en rojo, el eje Y en verde y el eje Z en azul.
 * La longitud de cada eje es de 1000 unidades, centrada en el origen (0,0,0).
 */
void dibujar_ejes() {
    glLineWidth(1.0f); // Líneas más finas

    glBegin(GL_LINES);
    // Eje X en rojo oscuro
    glColor3f(0.5f, 0.0f, 0.0f);
    glVertex3f(-500.0f, 0.0f, 0.0f);
    glVertex3f(500.0f, 0.0f, 0.0f);

    // Eje Y en verde oscuro
    glColor3f(0.0f, 0.5f, 0.0f);
    glVertex3f(0.0f, -500.0f, 0.0f);
    glVertex3f(0.0f, 500.0f, 0.0f);

    // Eje Z en azul oscuro
    glColor3f(0.0f, 0.0f, 0.5f);
    glVertex3f(0.0f, 0.0f, -500.0f);
    glVertex3f(0.0f, 0.0f, 500.0f);

    glEnd();
}

/**
 * Dibuja una malla de líneas en el plano XZ, centrada en el origen, con líneas
 * extendiéndose a lo largo del rango de -500 a 500 unidades en ambas direcciones.
 * Toma como parámetro el tamaño de cada cuadrícula de la malla,
 * permitiendo controlar la densidad de líneas.
 * Esta malla proporciona un fondo que puede ayudar a juzgar la distancia
 * y el movimiento relativo de los objetos en la escena.
 */
void dibujar_malla(float grid_size) {
    glColor3f(0.2f, 0.2f, 0.2f); // Gris oscuro
    glBegin(GL_LINES);

    for (float i = -500.0f; i <= 500.0f; i += grid_size) {
        glVertex3f(i, -500.0f, 0.0f);
        glVertex3f(i, 500.0f, 0.0f);
        glVertex3f(-500.0f, i, 0.0f);
        glVertex3f(500.0f, i, 0.0f);
    }

    glEnd();
}

/**
 * Dibuja los ejes locales de un objeto en el espacio de visualización, utilizando el centroide
 * del objeto como el origen de estos ejes. Los ejes se dibujan en las direcciones positivas
 * desde el centroide.
 * TO-DO: Arreglar, funciona correctamente si no hay camara - else - se escacharra.
 * @param obj Puntero a la estructura 'triobj' del objeto al que pertenecen los ejes.
 */
void dibujar_ejes_objeto(triobj *obj) {
    // TODO: Hacer que el centroide se calcule tan sólo una vez y asociar al propio objeto.
    Punto centro = calcular_centroide(obj);
    double longitud_eje = 80.0;
    double point_size = 5.0; // Tamaño del punto al final del eje

    // Guardar el tamaño del punto actual
    // Me estaba dando muchos problemas con la imagen, tengo que restaurarlo
    GLfloat prevPointSize;
    glGetFloatv(GL_POINT_SIZE, &prevPointSize);

    // Cada eje, 2 puntos (inicial - final)
    Punto eje_x[2], eje_y[2], eje_z[2];

    // Crear ejes en base al centroide
    eje_x[PUNTO_INICIAL] = (Punto){centro.x - longitud_eje, centro.y, centro.z};
    eje_x[PUNTO_FINAL] = (Punto){centro.x + longitud_eje, centro.y, centro.z};

    eje_y[PUNTO_INICIAL] = (Punto){centro.x, centro.y - longitud_eje, centro.z};
    eje_y[PUNTO_FINAL] = (Punto){centro.x, centro.y + longitud_eje, centro.z};

    eje_z[PUNTO_INICIAL] = (Punto){centro.x, centro.y, centro.z - longitud_eje};
    eje_z[PUNTO_FINAL] = (Punto){centro.x, centro.y, centro.z + longitud_eje};

    // TODO: Si hay cámara, que no le afecten las traslaciones.
    // Para que se quede en el centro indicando los ejes al menos.
    // El eje local tiene que tener las mismas transformaciones
    // que el objeto.
    mxp(&eje_x[PUNTO_INICIAL], obj->mptr->m, eje_x[PUNTO_INICIAL]);
    mxp(&eje_x[PUNTO_FINAL], obj->mptr->m, eje_x[PUNTO_FINAL]);
    mxp(&eje_y[PUNTO_INICIAL], obj->mptr->m, eje_y[PUNTO_INICIAL]);
    mxp(&eje_y[PUNTO_FINAL], obj->mptr->m, eje_y[PUNTO_FINAL]);
    mxp(&eje_z[PUNTO_INICIAL], obj->mptr->m, eje_z[PUNTO_INICIAL]);
    mxp(&eje_z[PUNTO_FINAL], obj->mptr->m, eje_z[PUNTO_FINAL]);

    // EJES
    glBegin(GL_LINES);
    // Eje X en rojo
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(eje_x[PUNTO_INICIAL].x, eje_x[PUNTO_INICIAL].y, eje_x[PUNTO_INICIAL].z);
    glVertex3f(eje_x[PUNTO_FINAL].x, eje_x[PUNTO_FINAL].y, eje_x[PUNTO_FINAL].z);

    // Eje Y en verde
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(eje_y[PUNTO_INICIAL].x, eje_y[PUNTO_INICIAL].y, eje_y[PUNTO_INICIAL].z);
    glVertex3f(eje_y[PUNTO_FINAL].x, eje_y[PUNTO_FINAL].y, eje_y[PUNTO_FINAL].z);

    // Eje Z en azul
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(eje_z[PUNTO_INICIAL].x, eje_z[PUNTO_INICIAL].y, eje_z[PUNTO_INICIAL].z);
    glVertex3f(eje_z[PUNTO_FINAL].x, eje_z[PUNTO_FINAL].y, eje_z[PUNTO_FINAL].z);
    glEnd();

    // Dibujar los puntos al final de los ejes.
    // Con intención de dar sensación de orientación
    // El hacer flechicas - imposible, me he vuelto loco.
    glPointSize(point_size); // Establecer tamaño del punto
    glBegin(GL_POINTS);
    // Punto al final del eje X
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(eje_x[PUNTO_FINAL].x, eje_x[PUNTO_FINAL].y, eje_x[PUNTO_FINAL].z);

    // Punto al final del eje Y
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(eje_y[PUNTO_FINAL].x, eje_y[PUNTO_FINAL].y, eje_y[PUNTO_FINAL].z);

    // Punto al final del eje Z
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(eje_z[PUNTO_FINAL].x, eje_z[PUNTO_FINAL].y, eje_z[PUNTO_FINAL].z);
    glEnd();
    glPointSize(prevPointSize); // Restaurar el tamaño del punto a su valor anterior
}

/**
 * Dibuja un vector normal a partir de un triángulo dado.
 * Ahora mismo lo hace desde el centroide/baricentro del polígono, en lugar del primer
 * vértice.
 * @param triangulo Puntero al triángulo del cual se calculará y dibujará el vector normal.
 * @param normal_vector Vector normal que se desea dibujar.
 */
void draw_vector(Triangulo* triangulo, Vector3 normal_vector) {
    // Centro del triangulo/poligono, de aquí nacerá el vector.
    Vector3 baricentro = compute_polygon_centroid(triangulo);
    //print_vector(&baricentro);
    // Calculo el punto final, +40 ha dicho Joseba.
    Vector3 end_point = {baricentro.x + normal_vector.x * PERSPECTIVE_FACTOR*4,
                         baricentro.y + normal_vector.y * PERSPECTIVE_FACTOR*4,
                         baricentro.z + normal_vector.z * PERSPECTIVE_FACTOR*4};

    // Establecer el color de la línea a amarillo
    glColor3f(1.0f, 1.0f, 0.0f);

    // Capturo el valor actual de linewidth antes de machacarlo.
    GLfloat prevLineWidth;
    glGetFloatv(GL_LINE_WIDTH, &prevLineWidth);
    glLineWidth(3.0f);

    // Dibujar la línea
    glBegin(GL_LINES);
    glVertex3f(baricentro.x, baricentro.y, baricentro.z);
    glVertex3f(end_point.x, end_point.y, end_point.z);
    glEnd();

    // Restablezco ahora el valor que tenía.
    glLineWidth(prevLineWidth);
}