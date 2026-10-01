# 🏥 Sistema de Gerenciamento de Fila Hospitalar Prioritária

Projeto desenvolvido para a disciplina de Estrutura de Dados com o objetivo de simular o funcionamento de uma fila de atendimento hospitalar utilizando estruturas eficientes para busca, inserção, remoção e ordenação.

---

## 📋 Objetivo

O sistema permite:

* Cadastrar pacientes
* Buscar pacientes pelo CPF
* Inserir pacientes em fila prioritária
* Atender pacientes por nível de risco
* Registrar histórico de atendimentos
* Ordenar históricos pelo tempo de espera
* Realizar benchmarks de desempenho

---

## 🚀 Tecnologias

* Linguagem C
* Alocação dinâmica de memória
* Tabela Hash
* Heap (Fila de Prioridade)
* Lazy Deletion
* Merge Sort
* Insertion Sort

---

## 🏗 Arquitetura do Projeto

```text
Paciente
    ↓
Tabela Hash
    ↓
Fila Prioritária (Heap)
    ↓
Atendimento
    ↓
Histórico
    ↓
Ordenação (R7)
```

---

## 📚 Estruturas Utilizadas

### 1. Paciente

Armazena:

* CPF
* Nome
* Data de nascimento
* Status na fila

```c
typedef struct {
    char cpf[15];
    char nome[100];
    char nascimento[11];
    int ativo_na_fila;
} Paciente;
```

---

### 2. Tabela Hash

Responsável por realizar buscas rápidas de pacientes utilizando o CPF como chave.

Complexidade:

| Operação | Complexidade |
| -------- | ------------ |
| Inserção | O(1)         |
| Busca    | O(1)         |
| Remoção  | O(1)         |

---

### 3. Heap (Fila de Prioridade)

Organiza pacientes segundo:

1. Grau de risco
2. Ordem de chegada

Exemplo:

```text
Risco 1 → prioridade máxima
Risco 5 → prioridade mínima
```

Complexidade:

| Operação | Complexidade |
| -------- | ------------ |
| Inserção | O(log n)     |
| Remoção  | O(log n)     |
| Consulta | O(1)         |

---

### 4. Lazy Deletion

Pacientes desistentes não são removidos imediatamente.

O elemento recebe:

```c
ativo = 0;
```

A remoção física ocorre apenas quando o elemento chega ao topo do Heap.

Vantagens:

* Menor custo computacional
* Evita reorganizações desnecessárias

---

## 🔄 Funcionalidades

### R1 - Cadastro

Cadastra um paciente no sistema.

---

### R2 - Busca

Busca paciente pelo CPF utilizando tabela hash.

---

### R3 - Entrada na fila

Insere paciente no Heap.

---

### R4 - Chamar próximo

Remove o paciente de maior prioridade.

---

### R5 - Desistência

Marca paciente como inativo.

---

### R6 - Tamanho da fila

Retorna quantidade de pacientes ativos.

---

### R7 - Ordenação do histórico

Implementa:

* Insertion Sort
* Merge Sort

Ordenação pelo maior tempo de espera.

---

## 📊 Benchmark

O projeto mede:

* Tempo de busca
* Tempo de atendimento
* Tempo de ordenação

Exemplo:

```text
Busca Hash:
0.00000002 s

Heap:
0.00000015 s

Merge Sort:
0.003 s

Insertion Sort:
0.840 s
```

---

## 📈 Complexidade Geral

| Operação    | Estrutura     | Complexidade |
| ----------- | ------------- | ------------ |
| Cadastro    | Hash          | O(1)         |
| Busca       | Hash          | O(1)         |
| Inserção    | Heap          | O(log n)     |
| Atendimento | Heap          | O(log n)     |
| Desistência | Lazy Deletion | O(1)         |
| Ordenação   | Merge Sort    | O(n log n)   |

---

## Código da Primeira Sessão

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char cpf[15];
    char nome[100];
    char nascimento[11];
}
Paciente;

typedef struct {
    Paciente paciente;
    int risco;
    int ordemEntrada;
} PacienteFila;

typedef struct {
    Paciente paciente;
    int risco;
    int ordemEntrada;
    int ordemAtendimento;
} PacienteAtendido;

Paciente *cadastro = NULL;
int totalPacientes = 0;

PacienteFila *fila = NULL;
int tamanhoFila = 0;

PacienteAtendido *atendidos = NULL;
int totalAtendidos = 0;

int relogio_eventos = 0;

int cadastrar(const char *cpf, const char *nome, const char *nascimento) {

    for (int i = 0; i < totalPacientes; i++) {

        if (strcmp(cadastro[i].cpf, cpf) == 0) {
            return 0;
        }
    }

    Paciente *temp = realloc(
        cadastro,
        (totalPacientes + 1) * sizeof(Paciente)
    );

    if (temp == NULL) {
        return 0;
    }

    cadastro = temp;

    strcpy(cadastro[totalPacientes].cpf, cpf);
    strcpy(cadastro[totalPacientes].nome, nome);
    strcpy(cadastro[totalPacientes].nascimento, nascimento);

    totalPacientes++;

    return 1;
}

Paciente *buscar_cadastro(const char *cpf) {

    for (int i = 0; i < totalPacientes; i++) {

        if (strcmp(cadastro[i].cpf, cpf) == 0) {
            return &cadastro[i];
        }
    }

    return NULL;
}

int dar_entrada(const char *cpf, int risco) {

    Paciente *paciente = buscar_cadastro(cpf);

    if (paciente == NULL) {
        return 0;
    }

    for (int i = 0; i < tamanhoFila; i++) {

        if (strcmp(fila[i].paciente.cpf, cpf) == 0) {
            return 0;
        }
    }

    PacienteFila *temp = realloc(
        fila,
        (tamanhoFila + 1) * sizeof(PacienteFila)
    );

    if (temp == NULL) {
        return 0;
    }

    fila = temp;

    fila[tamanhoFila].paciente = *paciente;
    fila[tamanhoFila].risco = risco;
    fila[tamanhoFila].ordemEntrada = relogio_eventos;
    relogio_eventos++;

    tamanhoFila++;

    return 1;
}

PacienteFila *chamar_proximo() {

    if (tamanhoFila == 0) {
        return NULL;
    }

    int indiceMelhor = 0;

    for (int i = 1; i < tamanhoFila; i++) {

        if (fila[i].risco < fila[indiceMelhor].risco) {

            indiceMelhor = i;

        } else if (
            fila[i].risco == fila[indiceMelhor].risco &&
            fila[i].ordemEntrada < fila[indiceMelhor].ordemEntrada
        ) {

            indiceMelhor = i;
        }
    }

    PacienteFila *resultado = malloc(sizeof(PacienteFila));

    if (resultado == NULL) {
        return NULL;
    }

    *resultado = fila[indiceMelhor];

    PacienteAtendido *temp = realloc(
        atendidos,
        (totalAtendidos + 1) * sizeof(PacienteAtendido)
    );

    if (temp == NULL) {
        free(resultado);
        return NULL;
    }

    atendidos = temp;

    atendidos[totalAtendidos].paciente = resultado->paciente;
    atendidos[totalAtendidos].risco = resultado->risco;
    atendidos[totalAtendidos].ordemEntrada =
        resultado->ordemEntrada;
    atendidos[totalAtendidos].ordemAtendimento = relogio_eventos;
    relogio_eventos++;

    totalAtendidos++;

    for (int i = indiceMelhor; i < tamanhoFila - 1; i++) {
        fila[i] = fila[i + 1];
    }

    tamanhoFila--;

    if (tamanhoFila == 0) {

        free(fila);
        fila = NULL;

    } else {

        PacienteFila *novaFila = realloc(
            fila,
            tamanhoFila * sizeof(PacienteFila)
        );

        if (novaFila != NULL) {
            fila = novaFila;
        }
    }

    return resultado;
}

int desistir(const char *cpf) {

    int indice = -1;

    for (int i = 0; i < tamanhoFila; i++) {

        if (strcmp(fila[i].paciente.cpf, cpf) == 0) {
            indice = i;
            break;
        }
    }

    if (indice == -1) {
        return 0;
    }

    for (int i = indice; i < tamanhoFila - 1; i++) {
        fila[i] = fila[i + 1];
    }

    tamanhoFila--;

    if (tamanhoFila == 0) {

        free(fila);
        fila = NULL;

    } else {

        PacienteFila *temp = realloc(
            fila,
            tamanhoFila * sizeof(PacienteFila)
        );

        if (temp != NULL) {
            fila = temp;
        }
    }

    relogio_eventos++;
    return 1;
}

int tamanho_fila() {
    return tamanhoFila;
}

void relatorio_do_dia() {

    printf("\n===== RELATORIO DO DIA =====\n");

    if (totalAtendidos == 0) {
        printf("Nenhum paciente atendido.\n");
        return;
    }

    for (int i = 0; i < totalAtendidos; i++) {

        for (int j = i + 1; j < totalAtendidos; j++) {

            int esperaI =
                atendidos[i].ordemAtendimento -
                atendidos[i].ordemEntrada;

            int esperaJ =
                atendidos[j].ordemAtendimento -
                atendidos[j].ordemEntrada;

            if (esperaJ > esperaI) {

                PacienteAtendido temp = atendidos[i];

                atendidos[i] = atendidos[j];

                atendidos[j] = temp;
            }
        }
    }

    for (int i = 0; i < totalAtendidos; i++) {

        int espera =
            atendidos[i].ordemAtendimento -
            atendidos[i].ordemEntrada;

        printf(
            "%d. %s | CPF: %s | Espera: %d eventos\n",
            i + 1,
            atendidos[i].paciente.nome,
            atendidos[i].paciente.cpf,
            espera
        );
    }
}

void liberar_memoria() {

    free(cadastro);
    free(fila);
    free(atendidos);

    cadastro = NULL;
    fila = NULL;
    atendidos = NULL;
}

int main() {

    cadastrar(
        "111.111.111-11",
        "Joao",
        "15/03/2000"
    );

    cadastrar(
        "222.222.222-22",
        "Maria",
        "20/05/1998"
    );

    cadastrar(
        "333.333.333-33",
        "Carlos",
        "10/10/1985"
    );

    cadastrar(
        "444.444.444-44",
        "Ana",
        "01/01/1990"
    );

    Paciente *paciente =
        buscar_cadastro("222.222.222-22");

    if (paciente != NULL) {

        printf(
            "Paciente encontrado: %s\n",
            paciente->nome
        );
    }

    dar_entrada("111.111.111-11", 3);
    dar_entrada("222.222.222-22", 1);
    dar_entrada("333.333.333-33", 2);
    dar_entrada("444.444.444-44", 1);

    printf(
        "\nTamanho da fila: %d\n",
        tamanho_fila()
    );

    PacienteFila *proximo = chamar_proximo();

    if (proximo != NULL) {

        printf(
            "Proximo paciente: %s | Risco: %d\n",
            proximo->paciente.nome,
            proximo->risco
        );

        free(proximo);
    }

    printf(
        "Tamanho da fila: %d\n",
        tamanho_fila()
    );

    if (desistir("333.333.333-33")) {

        printf("Paciente desistiu da fila.\n");

    } else {

        printf("Paciente nao encontrado na fila.\n");
    }

    printf(
        "Tamanho da fila: %d\n",
        tamanho_fila()
    );

    proximo = chamar_proximo();

    if (proximo != NULL) {

        printf(
            "Proximo paciente: %s | Risco: %d\n",
            proximo->paciente.nome,
            proximo->risco
        );

        free(proximo);
    }

    relatorio_do_dia();

    liberar_memoria();

    return 0;
}

---

Projeto acadêmico desenvolvido para estudo de:

* Estruturas de Dados
* Filas de Prioridade
* Tabelas Hash
* Algoritmos de Ordenação
* Análise de Complexidade

--- 

