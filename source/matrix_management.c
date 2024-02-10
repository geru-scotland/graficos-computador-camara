#include "headers/shared_defines.h"

/***********************************************************************
 *                                                                     *
 *                        GESTIÓN DE MATRICES                          *
 *                                                                     *
 ***********************************************************************/

/***********************************************************************
 * Este archivo se centra en la gestión de matrices, su funcionalidad
 * principal es la de crear nuevas matrices en memoria
 * dinámica y la de deshacer cambios en la estructura de datos
 * de matrices.
 ***********************************************************************/

/**
 * Crea y gestiona una nueva matriz en la lista vinculada de matrices para un objeto.
 *
 * @param sel_ptr Puntero al objeto cuyas matrices se están gestionando.
 * @return Puntero a la nueva matriz creada, o NULL si la creación falla.
 */
mlist* gestionar_nueva_matriz(triobj* sel_ptr) {
    // Creo un nuevo elemento mlist en memoria dinámica o heap
    mlist *nodo_matriz = (mlist *)malloc(sizeof(mlist));
    if (!nodo_matriz)
        return NULL;

    // Copio el contenido actual de sel_ptr->mptr->m a nodo_matriz->m
    memcpy(nodo_matriz->m, sel_ptr->mptr->m, sizeof(nodo_matriz->m));

    // nodo_matriz->hptr apunte al mlist anterior (el que estaba siendo apuntado por sel_ptr->mptr)
    nodo_matriz->hptr = sel_ptr->mptr;

    // Ahora hago que sel_ptr->mptr apunte al nuevo mlist
    sel_ptr->mptr = nodo_matriz;

    return nodo_matriz;
}

/**
 * Deshace la última operación de matriz, eliminando la matriz más reciente de la lista.
 *
 * @param sel_ptr Puntero al objeto cuya última operación de matriz se deshará.
 */
void undo(triobj* sel_ptr)
{
    // Primero verifico si hay un nodo anterior para deshacer
    if (sel_ptr->mptr && sel_ptr->mptr->hptr)
    {
        // Guardoun puntero temporal al nodo actual
        mlist *current_node = sel_ptr->mptr;

        // selptr->mptr al nodo anterior (hptr del nodo actual)
        sel_ptr->mptr = current_node->hptr;

        // Liberar memoria, será del sel_ptr->mptr anterior realmente.
        free(current_node);
    }
    else
    {
        printf("\nNo hay más acciones para deshacer.\n");
    }
}