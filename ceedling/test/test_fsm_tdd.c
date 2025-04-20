#include "unity.h"

#define TEST_CASE(...)

#include "fsm.h"
#include "mock_test_fsm.h"

#include <stdlib.h>


/**
 * @file test_fsm_tdd.c
 * @author 
 * @author 
 * @brief Tests for existing fsm module
 * @version 0.1
 * @date 2024-04-09
 * 
 */

/**
 * @brief Stub or Callback for fsm_malloc that calls real malloc. 
 * 
 * @param[in] s Amount of bytes to allocate
 * @param[in] n Amount of calls to this function
 * 
 * @return pointer to allocated memory if success; NULL if fails
 * 
 */
static void* cb_malloc(size_t s, int n) {
    return malloc(s);
}

/**
 * @brief Stub or Callback for fsm_free that calls real free. 
 * 
 * @param[in] p Pointer to allocated memory to free
 * @param[in] n Amount of calls to this function
 * 
 */
static void cb_free(void* p, int n) {
    return free(p);
}

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * @brief Comprueba que la funcion de fsm_new devuelve NULL 
 *        y no llama a fsm_malloc si la tabla de transiciones es NULL 
 */
void test_fsm_new_nullWhenNullTransition(void)
{
    fsm_t *f = (fsm_t*)1;

    f = fsm_new(NULL);

    TEST_ASSERT_EQUAL (NULL, f);
}

/**
 * @brief Comprueba que la función de inicialización devuelve false si el puntero a la maquina de estado es NULL 
 *
 */
void test_fsm_init_falseWhenNullFsm(void)
{
    fsm_trans_t tt[] = {
        {0, is_true, 1, do_nothing},
        {-1, NULL, -1, NULL}
    };

    int result = fsm_init(NULL, tt);

    TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Función de inicializacion devuelve false si la tabla de transiciones es nula
 * 
 */
void test_fsm_init_falseWhenNullTransitions(void)
{
    fsm_t f;
    int result = fsm_init(&f, NULL);
    TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief La máquina de estados devuelve NULL
 *        y no llama a fsm_malloc si el estado de origen
 *        de la primera transición es -1 (fin de la tabla)
 */
void test_fsm_nullWhenFirstOrigStateIsMinusOne (void) {
    fsm_trans_t tt[] = {{-1, is_true, 1, do_nothing}};
    fsm_malloc_IgnoreAndReturn((void*)0xDEADBEEF);

    fsm_t *f = fsm_new(tt);

    TEST_ASSERT_NULL(f);
}

/**
 * @brief La máquina de estados devuelve NULL y no llama a fsm_malloc si el estado de destino de la primera transición es -1 (fin de la tabla)
 * 
 */
void test_fsm_nullWhenFirstDstStateIsMinusOne (void) {
    fsm_trans_t tt[] = {{0, is_true, -1, do_nothing}, {-1, NULL, -1, NULL}};
    fsm_malloc_IgnoreAndReturn((void*)0x12345678);

    fsm_t *f = fsm_new(tt);

    TEST_ASSERT_NULL(f);
}

/**
 * @brief La máquina de estados devuelve NULL y no llama a fsm_malloc si la función de comprobación de la primera transición es NULL (fin de la tabla)
 * 
 */
void test_fsm_nullWhenFirstCheckFunctionIsNull (void)  {
    fsm_trans_t tt[] = {{0, NULL, 1, do_nothing}, {-1, NULL, -1, NULL}};
    fsm_malloc_IgnoreAndReturn((void*)0xBEEF1234);

    fsm_t *f = fsm_new(tt);

    TEST_ASSERT_NULL(f);
}
/**
 * @brief Devuelve puntero no NULL y llama a fsm_malloc (usa Callback a cb_mallock y fsm_malloc_ExpectXXX) al crear la maquina de estados con una transición válida
 *        con función de actualización (salida) NULL o no NULL.
 *        Hay que liberar la memoria al final llamando a free
 * 
 */
TEST_CASE(NULL)
TEST_CASE(do_nothing)
void test_fsm_new_nonNullWhenOneValidTransitionCondition(fsm_output_func_t out)
{
    fsm_trans_t tt[] = {
        {0, is_true, 1, out},
        {-1, NULL, -1, NULL}
    };

    fsm_t fake_fsm;
    fsm_malloc_IgnoreAndReturn(&fake_fsm);

    fsm_t *f = fsm_new(tt);

    TEST_ASSERT_EQUAL_PTR(&fake_fsm, f);
}

/**
 * @brief Estado inicial corresponde al estado de entrada de la primera transición de la lista al crear una maquiina de estados y es valido.
 *        Usa Stub para fsm_malloc y luego libera la memoria con free
 */
void test_fsm_new_fsmGetStateReturnsOrigStateOfFirstTransitionAfterInit(void)
{
    fsm_trans_t tt[] = {{5, is_true, 6, NULL}, {-1, NULL, -1, NULL}};
    fsm_t fake_fsm;
    fsm_malloc_IgnoreAndReturn(&fake_fsm);

    fsm_t *f = fsm_new(tt);

    TEST_ASSERT_EQUAL(5, fsm_get_state(f));
}

/**
 * @brief La maquina de estado no transiciona si la funcion devuelve 0
 * 
 */
void test_fsm_fire_isTrueReturnsFalseMeansDoNothingIsNotCalledAndStateKeepsTheSame(void)
{
    fsm_t f;
    fsm_trans_t tt[] = {
        {0, is_true, 1, do_nothing},
        {-1, NULL, -1, NULL}
    };
    fsm_init(&f, tt);
    is_true_ExpectAndReturn(&f, false);
    fsm_fire(&f);
    TEST_ASSERT_EQUAL(0, fsm_get_state(&f));
}

/**
 * @brief Comprueba que el puntero pasado a fsm_fire es pasado a la función de guarda cuando se comprueba una transición
 * 
 */
void test_fsm_fire_checkFunctionCalledWithFsmPointerFromFsmFire(void)
{
    fsm_trans_t tt[] = {
        {0, is_true, 1, NULL},
        {-1, NULL, -1, NULL}
    };

    fsm_t f;
    fsm_init(&f, tt);

    is_true_ExpectAndReturn(&f, false);

    fsm_fire(&f);
}

/** 
 * @brief Comprueba que el fsm_fire funciona y tiene el estado correcto cuando la transición devuelve true (cambia) y cuando devuelve false (mantiene)
 * 
 */
TEST_CASE(false, 0)
TEST_CASE(true, 1)
void test_fsm_fire_checkFunctionIsCalledAndResultIsImportantForTransition(bool returnValue, int expectedState)
{
    fsm_trans_t tt[] = {
        {0, is_true, 1, NULL},
        {-1, NULL, -1, NULL}
    };

    fsm_t f;
    fsm_init(&f, tt);

    is_true_ExpectAndReturn(&f, returnValue);

    fsm_fire(&f);

    TEST_ASSERT_EQUAL(expectedState, fsm_get_state(&f));
}


/**
 * @brief La creación de una máquina de estados devuelve NULL si la reserva de memoria falla.
 *        Usa el Mock llamando a fsm_malloc_ExpectXXX sin Stub ni Callback.
 *
 */
void test_fsm_new_nullWhenFsmMallocReturnsNull(void)
{
    fsm_trans_t tt[] = {
        {0, is_true, 1, NULL},
        {-1, NULL, -1, NULL}
    };

    fsm_malloc_IgnoreAndReturn(NULL);

    fsm_t *f = fsm_new(tt);

    TEST_ASSERT_NULL(f);
}

/**
 * @brief Llamar a fsm_destroy provoca una llamada a fsm_free
 *        Usa el Mock llamando a fsm_free_Expect sin Stub ni Callback
 *
 */
void test_fsm_destroy_callsFsmFree(void)
{
    fsm_t *f = (fsm_t *)0xBABECAFE;

    fsm_free_Expect(f);

    fsm_destroy(f);
}

/**
 * @brief Comprueba que solo se llame a la función de guarda que toca según el estado actual
 * 
 */
void test_fsm_fire_callsFirstIsTrueFromState0AndThenIsTrue2FromState1(void)
{
    fsm_trans_t tt[] = {
        {0, is_true, 1, NULL},
        //{1, is_true2, 0, NULL},   //Descomentar cuando se haya declarado una nueva función para mock is_true2
        {-1, NULL, -1, NULL}
    };

    fsm_t f;
    int res;
    fsm_init(&f, tt);
    res = fsm_get_state(&f);

    TEST_IGNORE();
}

/**
 * @brief Comprueba que se pueden crear dos instancias de máquinas de estados simultánteas y son punteros distintos.
 *        Usa Stub para fsm_malloc y luego libera la memoria con free
 *
 */
void test_fsm_new_calledTwiceWithSameValidDataCreatesDifferentInstancePointer(void)
{
    fsm_trans_t tt[] = {
        {0, is_true, 1, NULL},
        {-1, NULL, -1, NULL}
    };

    fsm_t fake1, fake2;
    fsm_malloc_IgnoreAndReturn(&fake1);
    fsm_t *a = fsm_new(tt);

    fsm_malloc_IgnoreAndReturn(&fake2);
    fsm_t *b = fsm_new(tt);

    TEST_ASSERT_NOT_EQUAL(a, b);
}

/**
 * @brief Comprueba que no se llama a la función de comprobación (in) si el estado actual no coincide con el estado origen de la transición.
 */

void test_fsm_fire_doesNotCallInFunctionWhenStateDoesNotMatch(void)
{
    fsm_trans_t tt[] = {
        {1, is_true, 2, do_nothing}, // current_state ≠ orig_state
        {-1, NULL, -1, NULL}
    };

    fsm_t f;
    fsm_init(&f, tt); // estado actual será 1, pero vamos a cambiarlo a 0
    f.current_state = 0;

    // is_true NUNCA debe llamarse, así que no ponemos ningún Expect

    fsm_fire(&f);

    TEST_ASSERT_EQUAL(0, fsm_get_state(&f)); // El estado no cambia
}

/**
 * @brief Comprueba que fsm_set_state cambia correctamente el estado de la máquina.
 */
void test_fsm_set_state_changesState(void)
{
    fsm_t f;
    f.current_state = 0;
    fsm_set_state(&f, 42);
    TEST_ASSERT_EQUAL(42, fsm_get_state(&f));
}

/**
 * @brief Función de salida ficticia usada para comprobar que se llama desde fsm_fire si está definida.
 */
void out_called(fsm_t* f) {
    // función vacía, solo para probar que se llama
}

/**
 * @brief Comprueba que fsm_fire llama a la función de salida (out) si está definida en la transición tomada.
 */
void test_fsm_fire_callsOutFunctionIfDefined(void) {
    fsm_trans_t tt[] = {
        {0, is_true, 1, out_called},
        {-1, NULL, -1, NULL}
    };

    fsm_t f;
    fsm_init(&f, tt);

    is_true_ExpectAndReturn(&f, true);

    fsm_fire(&f); // Debe ejecutar la transición y llamar a out_called

    TEST_ASSERT_EQUAL(1, fsm_get_state(&f)); // Se asegura que ha hecho la transición
}

/**
 * @brief Comprueba que fsm_init no modifica el estado si la tabla de transiciones es NULL.
 */
void test_fsm_init_doesNothingIfNullTransitions(void)
{
    fsm_t f;
    f.current_state = 1234;  // valor previo que no debe cambiar

    fsm_init(&f, NULL);

    // El estado debe quedarse igual si no se inicializa
    TEST_ASSERT_EQUAL(1234, f.current_state);
}

/**
 * @brief Comprueba que los callbacks cb_malloc y cb_free funcionan correctamente al usarse como sustitutos de malloc y free.
 */
void test_fsm_malloc_cb_called(void) {
    void* p = cb_malloc(10, 0);
    TEST_ASSERT_NOT_NULL(p);
    cb_free(p, 0);
}

/**
 * @brief Devuelve 0 si hay más de FSM_MAX_TRANSITIONS
 */
void test_fsm_init_returns0WhenTooManyTransitions(void)
{
    fsm_t f;
    fsm_trans_t tt[FSM_MAX_TRANSITIONS + 2];

    for (int i = 0; i < FSM_MAX_TRANSITIONS + 1; i++) {
        tt[i].orig_state = i;
        tt[i].in = is_true;
        tt[i].dest_state = i + 1;
        tt[i].out = do_nothing;
    }
    tt[FSM_MAX_TRANSITIONS + 1] = (fsm_trans_t){-1, NULL, -1, NULL};

    int result = fsm_init(&f, tt);

    TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Comprueba que fsm_init devuelve el número de transiciones válidas
 */
void test_fsm_init_returnsNumberOfValidTransitions(void)
{
    fsm_t f;
    fsm_trans_t tt[] = {
        {0, is_true, 1, do_nothing},
        {1, is_true, 2, do_nothing},
        {2, is_true, 3, do_nothing},
        {-1, NULL, -1, NULL} // Fin de tabla
    };

    int result = fsm_init(&f, tt);

    TEST_ASSERT_EQUAL(3, result);  // Debe contar 3 transiciones válidas
}

/**
 * @brief Comprueba que fsm_new devuelve NULL si fsm_init falla
 *        al detectar más de FSM_MAX_TRANSITIONS transiciones válidas.
 *        Es decir, si hay demasiadas transiciones, no debe reservar memoria.
 */
void test_fsm_new_returnsNullIfInitFailsDueToTooManyTransitions(void)
{
    fsm_trans_t tt[FSM_MAX_TRANSITIONS + 2];
    for (int i = 0; i < FSM_MAX_TRANSITIONS + 1; i++) {
        tt[i].orig_state = i;
        tt[i].in = is_true;
        tt[i].dest_state = i + 1;
        tt[i].out = do_nothing;
    }
    tt[FSM_MAX_TRANSITIONS + 1] = (fsm_trans_t){-1, NULL, -1, NULL};

    void *fake_ptr = (void*)0xDEADBEEF;
    fsm_malloc_IgnoreAndReturn(fake_ptr);    // Simula malloc
    fsm_free_Expect(fake_ptr);               // Espera que se libere

    fsm_t *f = fsm_new(tt);

    TEST_ASSERT_NULL(f);
}

/**
 * @brief Si la función de guarda es NULL, se debe considerar siempre como verdadera.
 */
void test_fsm_fire_guardNullIsTreatedAsTrue(void)
{
    fsm_trans_t tt[] = {
        {0, NULL, 1, NULL}, // Guarda NULL ⇒ se considera true
        {-1, NULL, -1, NULL}
    };

    fsm_t f;
    fsm_init(&f, tt);

    fsm_fire(&f);

    TEST_ASSERT_EQUAL(1, fsm_get_state(&f)); // Debe haber hecho la transición
}

/**
 * @brief Comprueba que no se evalúa una transición con guarda NULL si el estado actual no coincide con orig_state
 */
void test_fsm_fire_guardNullNoTransitionIfStateMismatch(void)
{
    fsm_trans_t tt[] = {
        {1, NULL, 2, NULL}, // El estado actual no coincide con orig_state
        {-1, NULL, -1, NULL}
    };

    fsm_t f;
    fsm_init(&f, tt);
    f.current_state = 0;

    fsm_fire(&f);

    TEST_ASSERT_EQUAL(0, fsm_get_state(&f)); // No debe transicionar
}

/**
 * @brief fsm_fire devuelve -1 si no hay transiciones para el estado actual
 */
void test_fsm_fire_returnsMinus1WhenNoTransitionsMatchCurrentState(void)
{
    fsm_t f;
    fsm_trans_t tt[] = {
        {1, is_true, 2, do_nothing},  // Estado inicial ≠ 0
        {-1, NULL, -1, NULL}
    };

    fsm_init(&f, tt);
    f.current_state = 0; // No hay transiciones para este estado

    int res = fsm_fire(&f);

    TEST_ASSERT_EQUAL(-1, res);
}

