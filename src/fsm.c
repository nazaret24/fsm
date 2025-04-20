/**
 * @file fsm.c
 * @brief Library to create Finite State Machines using composition.
 *
 * This library is expected to be used using composition
 * @author Teachers from the Departamento de Ingeniería Electrónica. Original authors: José M. Moya and Pedro J. Malagón. Latest contributor: Román Cárdenas.
 * @date 2023-09-20
 */

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdlib.h>

/* Other includes */
#include "fsm.h"

void* fsm_malloc(size_t s);
void  fsm_free(void* p);

fsm_t *fsm_new(fsm_trans_t *p_tt)
{
    if (p_tt == NULL)
    {
        return NULL;
    }

    if ((p_tt->orig_state == -1) || (p_tt->in == NULL) || (p_tt->dest_state == -1))
    {
        return NULL;
    }

    fsm_t *p_fsm = (fsm_t *)fsm_malloc(sizeof(fsm_t));
    if (p_fsm == NULL)
    {
        return NULL;
    }

    int num = fsm_init(p_fsm, p_tt);
    if (num == 0)
    {
        fsm_free(p_fsm);
        return NULL;
    }

    return p_fsm;
}

void fsm_destroy(fsm_t *p_fsm)
{
    fsm_free(p_fsm);
}

int fsm_init(fsm_t *p_fsm, fsm_trans_t *p_tt)
{
    if (p_fsm == NULL || p_tt == NULL) {
        return 0;
    }

    int count = 0;
    fsm_trans_t *t;

    for (t = p_tt; t->orig_state != -1; t++) {
        count++;
        if (count > FSM_MAX_TRANSITIONS) {
            return 0;
        }
    }

    p_fsm->p_tt = p_tt;
    p_fsm->current_state = p_tt->orig_state;

    return count;
}

int fsm_get_state(fsm_t *p_fsm)
{
    return p_fsm->current_state;
}

void fsm_set_state(fsm_t *p_fsm, int state)
{
    p_fsm->current_state = state;
}

int fsm_fire(fsm_t *p_fsm)
{
    fsm_trans_t *p_t;
    bool found = false;

    for (p_t = p_fsm->p_tt; p_t->orig_state >= 0; ++p_t)
    {
        if (p_t->orig_state == p_fsm->current_state)
        {
            found = true;

            // Si la función de guarda es NULL (siempre cierta) o devuelve true:
            if (p_t->in == NULL || p_t->in(p_fsm))
            {
                p_fsm->current_state = p_t->dest_state;

                if (p_t->out != NULL)
                {
                    p_t->out(p_fsm);
                }

                return 1; // Transición realizada
            }
        }
    }

    if (found)
        return 0;  // Hay transiciones desde el estado actual, pero ninguna válida
    else
        return -1; // Ninguna transición para el estado actual
}

// GCOVR_EXCL_START
void* __attribute__((weak)) fsm_malloc(size_t s) {
    return malloc(s);
}

void __attribute__((weak)) fsm_free(void* p) {
    free(p);
}
// GCOVR_EXCL_STOP

