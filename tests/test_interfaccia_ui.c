#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "interfaccia_ui.h"

void test_login(){
    op_cliente_t op = operazioni_login();
    assert(op == OP_CLI_LOGIN);
    printf("[OK] test_login passato.\n");
}

void test_registrazione(){
    op_cliente_t op = operazioni_login();
    assert(op == OP_CLI_REGISTRAZIONE);
    printf("[OK] test_registrazione passato.\n");
}

int main(){
    test_login();
    test_registrazione();

    return 0;
}

