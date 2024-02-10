//	Program developed by
//
//	Informatika Fakultatea
//	Euskal Herriko Unibertsitatea
//	http://www.ehu.eus/if
//
// to compile it: gcc dibujar-triangulos-y-objetos.c -lGL -lGLU -lglut
//
//
//
#include "headers/shared_defines.h"
#include "headers/cargar-triangulo.h"
#include "headers/functions.h"

#define GL_SILENCE_DEPRECATION

// testuraren informazioa
// información de textura
extern int load_ppm(char *file, unsigned char **bufferptr, int *dimxptr, int * dimyptr);
unsigned char *bufferra;
int dimx,dimy;

int indexx;
Triangulo *triangulosptr;
triobj *foptr;
triobj *sel_ptr;
int denak;
int lineak;
int objektuak;
char aldaketa;
int ald_lokala;
char fitxiz[100];

/**
 * Variable de control de estado que utiliza bits para representar y combinar
 * diferentes modos de operación, modos de cámara etc.
 * He preferido hacer los checks mediante variable bitwise, dada la cantidad
 * de combinaciones posibles que hay.
 */
unsigned int scene_status_mask = 0;

/**
 *Cámaras y datos de inicialización para los vectores y matriz de vista.
 */
Camera* main_camera;
// Auxiliar, para hacer "swap" a cámara objeto y poder "volver".
Camera* secondary_camera;

const CameraConst CameraData = {
    .EYE_POSITION = {0, 0, 500},
    .UP_VECTOR = {0, 1, 0},
    .LOOK_AT = {0, 0, 0},
};

void objektuari_aldaketa_sartu_ezk(double m[16]){}
void objektuari_aldaketa_sartu_esk(double m[16]){}

// funtzio honek u eta v koordenatuei dagokien pointerra itzuli behar du.
// debe devolver el pointer correspondiente a las coordenadas u y v
unsigned char* color_textura(float u, float v)
{
    int index = (v * dimx + u) * 3; // Cada píxel tiene 3 bytes (R, G, B).
    return &bufferra[index];
}

// lerroa marrazten du, baina testuraren kodea egokitu behar da
// dibuja una linea pero hay que codificar la textura
void dibujar_linea_z(float linea,float c1x, float c1z, float c1u,float c1v,float c2x,float c2z,float c2u,float c2v)
{
    float xkoord, zkoord, u, v;
    unsigned char r, g, b;
    unsigned char* colorv;
    float u_incr, v_incr;
    int u_textura, v_textura;

    // Calcular los incrementos para u y v en cada paso del bucle.
    u_incr = (c2u - c1u) / (c2x - c1x);
    v_incr = (c2v - c1v) / (c2x - c1x);

    glBegin(GL_POINTS);
    for (xkoord = c1x, zkoord = c1z, u = c1u, v = c1v; xkoord <= c2x; xkoord++, u += u_incr, v += v_incr)
    {
        /*
          Convierto las coordenadas u, v a índices de pixel en la textura
          Incialmente probé con (c1u + (xkoord-c1x)*u_incr)*dimx y aunque tiene sentido
          genera crashes. Algo se me ha escapado.
        */
        u_textura = u*(dimx-1); // u aplicará la proporción sobre las dimensiones en px de tex
        v_textura = (1-v) * (dimy-1); // buffer 1-v, empieza arriba

        colorv = color_textura(u_textura, v_textura);
        r = colorv[0];
        g = colorv[1];
        b = colorv[2];
        glColor3ub(r, g, b);
        glVertex3f(xkoord, linea, zkoord);
    }
    glEnd();
}

void dibujar_triangulo(triobj *optr, int i)
{
    Triangulo *tptr;

    Punto *pgoiptr, *pbeheptr, *perdiptr;
    float x1,h1,z1,u1,v1,x2,h2,z2,u2,v2,x3,h3,z3,u3,v3;
    float c1x,c1z,c1u,c1v,c2x,c2z,c2u,c2v;
    int linea;
    float y_rasterization;
    float cambio1,cambio1z,cambio1u,cambio1v,cambio2,cambio2z,cambio2u,cambio2v;
    Punto p1,p2,p3;
    Punto pc1 = {0}, pc2 = {0}, p4 = {0};


    if (i >= optr->num_triangles)
        return;

    // Primer triangulo apuntado + desplazamiento
    //printf("\n i: %i, indexx: %i \n", i, indexx);
    tptr = optr->triptr + i;

    /**
     * Aqui se procesa la matriz de transformación que actualmente
     * sea la primera para cada triángulo.
     */
    Triangulo triangulo, triangulo_procesado = {};
    triangulo.p1 = tptr->p1;
    triangulo.p2 = tptr->p2;
    triangulo.p3 = tptr->p3;

    // Procesa el triángulo actual a través de la pipeline de la cámara,
    // aplicando transformaciones de modelo, vista y proyección.
    camera_pipeline(main_camera, scene_status_mask, &triangulo_procesado, &triangulo, optr->mptr->m);

    p1 = triangulo_procesado.p1;
    p2 = triangulo_procesado.p2;
    p3 = triangulo_procesado.p3;

#ifdef DEBUG
    mostrar_transformaciones();
#endif

    // Vectores normales de éste triángulo.
    //  1) Obtengo 2 vectores del polígono
    //  2) Hago su cross product
    //  3) El resultado será su vector normal.
    Vector3 normal_vector;
    obtain_normal_vector(&triangulo_procesado, &normal_vector);
    // Si el producto escalar del vector normal y la dirección de proyección
    // es positivo, dibujar el poligono, si no fuera, break.
    if(scene_status_mask & BACK_CULLING && !should_draw_polygon(normal_vector, main_camera->eye_position)) {
#ifdef DEBUG
        printf("\n OMITIENDO POLIGONO! \n");
#endif
        return;
    }

    // Dibujar vectores normales si se ha activado la opción (tecla "n")
    if(scene_status_mask & NORMAL_VECTORS) {
        draw_vector(&triangulo_procesado, normal_vector);
    }

    // TODO: NOTA PARA LOS OBJ, QUE TE COJA DE 3 EN 3 LOS VÉRTICES. UN FOR Y ALE, SI TIENE 6 PPOLIGONOS, QUE TE
    // VAYA "CREANDO" TRINAGULOS DE LOS VÉRTICES.

    // para implementar los focos etc, calcualr todos los vectores
    // en el mismo sistema de referencia TODO, no mezcles dice Joseba


    if (lineak == 1)
    {
        glLineWidth(1.0);
        glColor3f(0.6, 0.6, 0.6); // RGB para gris claro
        glBegin(GL_POLYGON);
        glVertex3d(p1.x, p1.y, p1.z);
        glVertex3d(p2.x, p2.y, p2.z);
        glVertex3d(p3.x, p3.y, p3.z);
        glEnd();


        glBegin(GL_LINE_LOOP);
        // Los mismos vértices para dibujar el contorno del polígono
        glVertex3d(p1.x, p1.y, p1.z);
        glVertex3d(p2.x, p2.y, p2.z);
        glVertex3d(p3.x, p3.y, p3.z);
        glEnd();
        return;
    }

    // [Corrección Discretización] Comprobar si los 3 vértices tienen misma altura
    if (p1.y == p2.y && p1.y == p3.y) {
        // Si es así, ordena los puntos por la coordenada x
        ordenar_puntos_x(&p1, &p2, &p3);
        // Dibuja desde el vértice izquierdo hasta el derecho
        dibujar_linea_z(p1.y, p1.x, p1.z, p1.u, p1.v, p3.x, p3.z, p3.u, p3.v);
        return;
    }

    // Determinar orden de puntos, superior, mediano e inferior.
    ordenar_puntos(&p1, &p2, &p3);

    p4.y = p2.y;

    /*
      Hago una doble interpolación lineal. Primero entre p1 y p3 y luego para el nuevo punto
      con respecto a p1, calculo múltiples puntos internos.
      Ahora hallo la x del nuevo punto p4, con interpolación lineal entre p1 y p3
    */
    interpolacion_lineal(&p1, &p3, &p4);

    // Antes de comenzar, la variable y_rasterization tiene que comenzar a rasterizar desde el p1, hasta el y correspondiente.
    y_rasterization = p1.y;

    // Triangulo parte superior
    while(y_rasterization >= p4.y){

        /*
          Ahora, ir detectando los segmentos, tengo ya los dos puntos.
          Hago uso de interpolación lineal. Por ejemplo, para el p1 - p4 y p1 y p2
          Y va a hacer de "Barra deslizadora", reduciendo el y hasta que sea mayor o igual a p4.y
        */
        pc1.y = y_rasterization;
        pc2.y = y_rasterization;
        interpolacion_lineal(&p1, &p4, &pc1);
        interpolacion_lineal(&p1, &p2, &pc2);

        //Dibujar segmento
        if(pc1.x < pc2.x) {
            dibujar_linea_z(y_rasterization, pc1.x, pc1.z, pc1.u, pc1.v, pc2.x, pc2.z, pc2.u, pc2.v);
        } else {
            dibujar_linea_z(y_rasterization, pc2.x, pc2.z, pc2.u, pc2.v, pc1.x, pc1.z, pc1.u, pc1.v);
        }

        y_rasterization--;
    }

    /*
       y_rasterization tiene ahora el valor de p4.y ó p2.y, que siga hasta p3.y
       Triangulo parte inferior
     */
    while(y_rasterization >= p3.y){

        pc1.y = y_rasterization;
        pc2.y = y_rasterization;

        interpolacion_lineal(&p3, &p4, &pc1);
        interpolacion_lineal(&p2, &p3, &pc2);

        // Dibujar segmento
        if(pc1.x < pc2.x) {
            dibujar_linea_z(y_rasterization, pc1.x, pc1.z, pc1.u, pc1.v, pc2.x, pc2.z, pc2.u, pc2.v);
        } else {
            dibujar_linea_z(y_rasterization, pc2.x, pc2.z, pc2.u, pc2.v, pc1.x, pc1.z, pc1.u, pc1.v);
        }

        y_rasterization--;
    }
}

static void marraztu(void)
{
    float u,v;
    int i,j;
    triobj *auxptr;
    /*
    unsigned char* colorv;
    unsigned char r,g,b;
    */

  // marrazteko objektuak behar dira
  // no se puede dibujar sin objetos
    if (foptr ==0)
        return;


    // clear viewport...
    if (objektuak == 1) glClear( GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT );
    else
    {
        if (denak == 0) glClear( GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT );
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-500.0, 500.0, -500.0, 500.0,-500.0, 500.0);

    triangulosptr = sel_ptr->triptr;
    if (objektuak == 1)
    {
        if (denak == 1)
        {
            for (auxptr = foptr; auxptr != 0; auxptr = auxptr->hptr)
            {
                // No mostrar el objeto actual, por ahora.
                // Borrar cuando se implemente proyección persepectiva
                if(scene_status_mask & MODO_OBJETO && auxptr == sel_ptr)
                    continue;

                for (i =0; i < auxptr->num_triangles; i++)
                {
                    dibujar_triangulo(auxptr, i);
                }
            }
        }
        else
          {
            for (i =0; i < sel_ptr->num_triangles; i++)
                {
                dibujar_triangulo(sel_ptr, i);
                }
          }
    }
    else
    {
         dibujar_triangulo(sel_ptr, indexx);
    }

    dibujar_ejes();
    dibujar_malla(28.0f);
    // Truco para que el eje local aparezca delante del objeto
    glDisable(GL_DEPTH_TEST);
    //dibujar_ejes_objeto(foptr); // Dibuja los ejes
    glEnable(GL_DEPTH_TEST);
    glFlush();
}


void read_from_file(char *fitx)
{
    int i,retval;
    triobj *optr;

    //printf("%s fitxategitik datuak hartzera\n",fitx);
    optr = (triobj *)malloc(sizeof(triobj));
    retval = cargar_triangulos(fitx, &(optr->num_triangles), &(optr->triptr));

    if (retval !=1)
    {
         printf("%s fitxategitik datuak hartzerakoan arazoak izan ditut\n    Problemas al leer\n",fitxiz);
         free(optr);
    }
    else
    {
         // Puntero a lista de triángulos, se inicializa
         triangulosptr = optr->triptr;
         //printf("objektuaren matrizea...\n");
         optr->mptr = (mlist *)malloc(sizeof(mlist));

         for (i=0; i<16; i++)
             optr->mptr->m[i] = 0;

         // Matriz de transformacion inicial es la identidad
         optr->mptr->m[0] = 1.0;
         optr->mptr->m[5] = 1.0;
         optr->mptr->m[10] = 1.0;
         optr->mptr->m[15] = 1.0;
         optr->mptr->hptr = 0;
         //printf("objektu zerrendara doa informazioa...\n");
         optr->hptr = foptr;

         //
         foptr = optr;
         sel_ptr = optr;
    }
     printf("datuak irakurrita\nLecura finalizada\n");
}

// No las utilizo por ahora, en su lugar: transformar
void x_aldaketa(int dir) {}
void y_aldaketa(int dir) {}
void z_aldaketa(int dir) {}


// This function will be called whenever the user pushes one key
static void teklatua(unsigned char key, int x, int y) {
    switch (key) {
        case 13: // Enter
            if (foptr != 0) {
                indexx++;
                if (indexx == sel_ptr->num_triangles) {
                    indexx = 0;
                    if (denak == 1 && objektuak == 0) {
                        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
                        glFlush();
                    }
                }
            }
            break;
        case 'd':
            denak = !denak;
            break;
        case 'o':
            objektuak = !objektuak;
            break;
        case 'l':
            lineak = !lineak;
            break;
        case 'j':
            print_camera_data(main_camera);
            break;
        case 'i':
            print_transformations(sel_ptr);
            break;
        case 'k': // [DEBUGGING]: Reset flags
        case 'K':
            scene_status_mask = 0;
            break;
        case 't':
            scene_status_mask &= ~EJE_LIMPIAR_MASK_TRANSFORMACION;
            scene_status_mask |= MODO_TRASLACION;
            break;
        case 'r':
            scene_status_mask &= ~EJE_LIMPIAR_MASK_TRANSFORMACION;
            scene_status_mask |= MODO_ROTACION;
            break;
        case 's':
            scene_status_mask &= ~EJE_LIMPIAR_MASK_TRANSFORMACION;
            scene_status_mask |= MODO_ESCALADO;
            break;
        case 'c':
            // No aceptar input si estamos en cámara objeto.
            if(scene_status_mask & MODO_OBJETO)
                break;
            //print_camera_data(main_camera);
            if(!(scene_status_mask & MODO_CAMARA))
                scene_status_mask |= MODO_CAMARA | CAMARA_VUELO;
            break;
        case 'C':
            if(scene_status_mask & MODO_OBJETO)
                scene_status_mask &= ~EJE_LIMPIAR_MASK_CAMARA;
            else
                scene_status_mask |= MODO_OBJETO;
            swap_camera(scene_status_mask, sel_ptr, main_camera, secondary_camera);
            break;
        case 'g':
            if(scene_status_mask & MODO_CAMARA){
                scene_status_mask &= ~CAMARA_ANALISIS;
                scene_status_mask |= CAMARA_VUELO;
                scene_status_mask |= EJE_LOCAL;
                break;
            }
            if(scene_status_mask & EJE_LOCAL)
                scene_status_mask &= ~EJE_LOCAL;
            else
                scene_status_mask |= EJE_LOCAL;
            break;
        case 'G':
            /**
             * Cámara modo análisis.
             *
             * Cuando se presiona la tecla 'G', la cámara se reorienta para enfocar al objeto (sel_ptr).
             * Primero, calculo el centroide del objeto para obtener un punto focal.
             * Despues convierto este punto en un vector (obj_position_vector) que representa la nueva dirección en la que la cámara tine que mirar.
             * Se actualiza la cámara utilizando update_camera, pasándole la posición actual de la cámara, el nuevo vector de enfoque o look at
             * y el vector up de la cámara. Por último, update_camera_vectors ajusta los vectores internos de la cámara para alinearla correctamente con
             * el nuevo focus, para que mire al objeto y se reoriente correctamente.
             *
             * Tengo pendiente mejorar esto para que el update_camera haga todo internamente.
             */
            if(scene_status_mask & MODO_CAMARA){                
                if(sel_ptr != NULL) {                    
                    scene_status_mask &= ~CAMARA_VUELO;
                    scene_status_mask &= ~EJE_LOCAL;
                    scene_status_mask |= CAMARA_ANALISIS;

                    // El centroide del objeto seleccionado será el nuevo punto de mira de la cámara.
                    Vector3 obj_position_vector;
                    Punto obj_position_point = calcular_centroide(sel_ptr);

                    point_to_vector(obj_position_point, &obj_position_vector);

                    // Updateo la posición de la cámara para que mire hacia el centroide del objeto.
                    update_camera(main_camera, main_camera->eye_position, obj_position_vector, main_camera->vector_up);

                    // Updatear los vectores, para que se alineen con el nuevo objeto-focus.
                    update_camera_vectors(main_camera, obj_position_vector);
                }
                break;
            }
            break;
        case 'x':
        case 'X':
        case 'y':
        case 'Y':
            // Bloquear rotación en XY
            if(scene_status_mask & MODO_ROTACION && scene_status_mask & CAMARA_VUELO)
                break;
            // Bloquear rotación para XY en modo análisis
            if(scene_status_mask & MODO_ROTACION && scene_status_mask & CAMARA_ANALISIS)
                break;
        case 'z':
        case 'Z': {
            /**
             * Al presionar estas teclas, se inicia una serie de acciones para transformar el objeto actual.
             * Primero, se determina la dirección de la transformación basada en si la tecla presionada es mayúscula o minúscula.
             * Luego, se limpia la máscara de estado de la escena de cualquier eje activo previo y se establece el eje correspondiente a la tecla presionada (X, Y, Z) en la máscara de estado.
             * Ahora, se llama a la función transformar ya con el eje y la dirección adecuados, junto con la cámara actual, la máscara de estado y el objeto
             */
            //print_scene_mask(&scene_status_mask);
            int direction = (key == 'X' || key == 'Y' || key == 'Z') ? -1 : 1;
            scene_status_mask &= ~EJE_LIMPIAR_MASK_EJES;
            scene_status_mask |= (key == 'x' || key == 'X') ? EJE_X_POSITIVO : (key == 'y' || key == 'Y') ? EJE_Y_POSITIVO
                                                                                                       : EJE_Z_POSITIVO;
            if (key == 'x' || key == 'X')
                transformar('x', direction, main_camera, scene_status_mask, sel_ptr);
            if (key == 'y' || key == 'Y')
                transformar('y', direction, main_camera, scene_status_mask, sel_ptr);
            if (key == 'z' || key == 'Z')
                transformar('z', direction, main_camera, scene_status_mask, sel_ptr);
            break;
        }
        case 'U':
        case 'u':
            undo(sel_ptr);
            break;
        case 'p':
            // "toggle" de persectivas
            if (scene_status_mask & PROJECTION_PERSPECTIVE) {
                scene_status_mask &= ~PROJECTION_PERSPECTIVE;  // Desactiva perspectiva
                scene_status_mask |= PROJECTION_ORTOGRAPHIC;   // Activa ortográfica
            } else {
                scene_status_mask &= ~PROJECTION_ORTOGRAPHIC;  // Desactiva ortográfica
                scene_status_mask |= PROJECTION_PERSPECTIVE;   // Activa perspectiva
            }
        break;
        case 'n':
            // Bitwise Toggle, manera más compacta de togglear.
            scene_status_mask ^= NORMAL_VECTORS;
            break;
        case 'b':
            scene_status_mask ^= BACK_CULLING;
            break;
        case 'f':
            printf("idatzi fitxategi izena\n");
            scanf("%s", fitxiz);
            read_from_file(fitxiz);
            indexx = 0;
            break;
        case 9: // <TAB>
            if (foptr != 0) {
                sel_ptr = sel_ptr->hptr;
                if (sel_ptr == 0) sel_ptr = foptr;
                indexx = 0; // the selected polygon is the first one
            }
            break;
        case 27: // <ESC>
            exit(0);
            break;
        default:
            printf("%d %c\n", key, key);
            break;
    }

    // The screen must be redrawn to show the new triangle
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    int retval;
    denak = 0;
    lineak =0;
    objektuak = 1;
    foptr = 0;
    sel_ptr = 0;
    aldaketa = 'r';
    ald_lokala = 1;

    /**
     * Inicializar las matrices Vista
     * y las cámaras a las que se asocian.
     */
    Camera global_camera;
    Camera aux_camera;

    View global_view;
    View aux_view;

    global_camera.view = &global_view;
    aux_camera.view = &aux_view;

    main_camera = &global_camera;
    secondary_camera = &aux_camera;

    // Valores iniciales de la máscara de estado de escena
    scene_status_mask |= PROJECTION_PERSPECTIVE;

	printf(" Triangeluak: barneko puntuak eta testura\n Triángulos con puntos internos y textura \n");
	printf("Press <ESC> to finish\n");
	glutInit(&argc,argv);
	glutInitDisplayMode ( GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize ( 500, 500 );
	glutInitWindowPosition ( 100, 100 );
	glutCreateWindow( "KBG/GO praktika" );

	glutDisplayFunc( marraztu );
	glutKeyboardFunc( teklatua );
    /* we put the information of the texture in the buffer pointed by bufferra. The dimensions of the texture are loaded into dimx and dimy */
    retval = load_ppm("testura.ppm", &bufferra, &dimx, &dimy);
    if (retval !=1)
    {
        printf("Ez dago texturaren fitxategia (testura.ppm)\n");
        exit(-1);
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // RGB negro y alfa (opacidad) completo

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST); // activar el test de profundidad (Z-buffer)

    if (argc>1)
        read_from_file(argv[1]);
    else{
        read_from_file("z.txt");
        read_from_file("z.txt");
        read_from_file("z.txt");
    }

    update_camera(secondary_camera, CameraData.EYE_POSITION, CameraData.LOOK_AT, CameraData.UP_VECTOR);
    update_camera(main_camera, CameraData.EYE_POSITION, CameraData.LOOK_AT, CameraData.UP_VECTOR);
	glutMainLoop();

	return 0;
}
