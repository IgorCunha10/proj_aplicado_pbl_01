#ifndef _WIN32
#define _POSIX_C_SOURCE 199309L
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define HASH_SIZE 200003

// ============================================================================
// ESTRUTURA DO PACIENTE
// ============================================================================

typedef struct {
    char cpf[15];
    char nome[100];
    char nascimento[11];
    int ativo_na_fila;
} Paciente;

// ============================================================================
// ESTRUTURA DA TABELA HASH
// ============================================================================

typedef struct HashNode {
    Paciente paciente;
    struct HashNode *prox;
} HashNode;

// ============================================================================
// ESTRUTURA PARA HISTÓRICO DE ATENDIDOS
// ============================================================================

typedef struct {
    Paciente paciente;
    int risco;
    int ordemEntrada;
    int ordemAtendimento;
} PacienteAtendido;

// ============================================================================
// CADASTRO
// ============================================================================

Paciente *cadastro = NULL;
int totalPacientes = 0;

PacienteAtendido *atendidos = NULL;
int totalAtendidos = 0;
int atendidosCap = 0;

int relogio_eventos = 0;

// ============================================================================
// ESTRUTURAS DO HEAP E LAZY DELETION
// ============================================================================

typedef struct {
    Paciente paciente;
    int risco;
    int ordemEntrada;
    int ativo;
} ElementoHeap;

// ============================================================================
// MIN-HEAP
// ============================================================================

typedef struct {
    ElementoHeap *dados;
    int tamanho;
    int capacidade;
} HeapFila;

HeapFila *fila = NULL;

int fila_ativos = 0;

// ============================================================================
// TABELA HASH
// ============================================================================

HashNode *tabela_hash[HASH_SIZE];

// ============================================================================
// FUNÇÃO PARA INICIALIZAR O HEAP
// ============================================================================

HeapFila* criar_heap(int capacidadeInicial) {
    HeapFila *h = (HeapFila*) malloc(sizeof(HeapFila));
    if (!h) return NULL;

    h->dados = (ElementoHeap*) malloc(capacidadeInicial * sizeof(ElementoHeap));
    if (!h->dados) {
        free(h);
        return NULL;
    }
    h->tamanho = 0;
    h->capacidade = capacidadeInicial;
    return h;
}

// ============================================================================
// COMPARADOR DE PRIORIDADE
// ============================================================================

int tem_maior_prioridade(const ElementoHeap *a, const ElementoHeap *b) {
    if (a->risco != b->risco) {
        return a->risco < b->risco;
    }
    return a->ordemEntrada < b->ordemEntrada;
}

// ============================================================================
// TROCAR ELEMENTOS
// ============================================================================

void trocar_elementos(ElementoHeap *a, ElementoHeap *b) {
    ElementoHeap temp = *a;
    *a = *b;
    *b = temp;
}

// ============================================================================
// SIFT-UP
// ============================================================================

void subir_heap(HeapFila *h, int idx) {
    while (idx > 0) {
        int pai = (idx - 1) / 2;
        if (tem_maior_prioridade(&h->dados[idx], &h->dados[pai])) {
            trocar_elementos(&h->dados[idx], &h->dados[pai]);
            idx = pai;
        } else {
            break;
        }
    }
}

// ============================================================================
// SIFT-DOWN
// ============================================================================

void descer_heap(HeapFila *h, int idx) {
    int menor = idx;
    int esq = 2 * idx + 1;
    int dir = 2 * idx + 2;

    if (esq < h->tamanho && tem_maior_prioridade(&h->dados[esq], &h->dados[menor])) {
        menor = esq;
    }
    if (dir < h->tamanho && tem_maior_prioridade(&h->dados[dir], &h->dados[menor])) {
        menor = dir;
    }
    if (menor != idx) {
        trocar_elementos(&h->dados[idx], &h->dados[menor]);
        descer_heap(h, menor);
    }
}

// ============================================================================
// CADASTRO ORIGINAL
// ============================================================================

int cadastrar_vetor(const char *cpf, const char *nome, const char *nascimento) {
    if (strlen(cpf) >= sizeof(cadastro[0].cpf) ||
        strlen(nome) >= sizeof(cadastro[0].nome) ||
        strlen(nascimento) >= sizeof(cadastro[0].nascimento)) {
        return 0;
    }

    for (int i = 0; i < totalPacientes; i++) {
        if (strcmp(cadastro[i].cpf, cpf) == 0) return 0;
    }

    Paciente *temp = realloc(cadastro, (totalPacientes + 1) * sizeof(Paciente));
    if (temp == NULL) return 0;
    cadastro = temp;

    snprintf(cadastro[totalPacientes].cpf, sizeof(cadastro[totalPacientes].cpf), "%s", cpf);
    snprintf(cadastro[totalPacientes].nome, sizeof(cadastro[totalPacientes].nome), "%s", nome);
    snprintf(cadastro[totalPacientes].nascimento, sizeof(cadastro[totalPacientes].nascimento), "%s", nascimento);
    cadastro[totalPacientes].ativo_na_fila = 0;

    totalPacientes++;
    return 1;
}

// ============================================================================
// FUNÇÃO HASH
// ============================================================================

unsigned long hash_function(const char *cpf) {
    unsigned long hash = 5381;
    int c;
    while ((c = *cpf++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % HASH_SIZE;
}

// ============================================================================
// BUSCAR CADASTRO
// ============================================================================

Paciente *buscar_cadastro(const char *cpf) {
    unsigned long indice = hash_function(cpf);
    HashNode *atual = tabela_hash[indice];

    while (atual != NULL) {
        if (strcmp(atual->paciente.cpf, cpf) == 0) {
            return &(atual->paciente);
        }
        atual = atual->prox;
    }
    return NULL;
}

// ============================================================================
// CADASTRAR NA TABELA HASH
// ============================================================================

int cadastrar(const char *cpf, const char *nome, const char *nascimento) {
    if (strlen(cpf) >= sizeof(((Paciente*)0)->cpf) ||
        strlen(nome) >= sizeof(((Paciente*)0)->nome) ||
        strlen(nascimento) >= sizeof(((Paciente*)0)->nascimento)) {
        return 0;
    }

    if (buscar_cadastro(cpf) != NULL) return 0;

    unsigned long indice = hash_function(cpf);
    HashNode *novo = malloc(sizeof(HashNode));
    if (novo == NULL) return 0;

    snprintf(novo->paciente.cpf, sizeof(novo->paciente.cpf), "%s", cpf);
    snprintf(novo->paciente.nome, sizeof(novo->paciente.nome), "%s", nome);
    snprintf(novo->paciente.nascimento, sizeof(novo->paciente.nascimento), "%s", nascimento);
    novo->paciente.ativo_na_fila = 0;

    novo->prox = tabela_hash[indice];
    tabela_hash[indice] = novo;
    return 1;
}

// ============================================================================
// R3 - DAR ENTRADA
// ============================================================================

int dar_entrada(const char *cpf, int risco) {
    Paciente *paciente = buscar_cadastro(cpf);
    if (paciente == NULL) return 0;
    if (paciente->ativo_na_fila == 1) return 0;

    if (fila == NULL) {
        fila = criar_heap(10);
        if (fila == NULL) return 0;
    }

    if (fila->tamanho == fila->capacidade) {
        int novaCapacidade = fila->capacidade * 2;
        ElementoHeap *temp = realloc(fila->dados, novaCapacidade * sizeof(ElementoHeap));
        if (!temp) return 0;
        fila->dados = temp;
        fila->capacidade = novaCapacidade;
    }

    int pos = fila->tamanho;
    paciente->ativo_na_fila = 1;

    fila->dados[pos].paciente = *paciente;
    fila->dados[pos].risco = risco;
    fila->dados[pos].ordemEntrada = relogio_eventos;
    fila->dados[pos].ativo = 1;

    relogio_eventos++;
    fila->tamanho++;
    fila_ativos++;

    subir_heap(fila, pos);
    return 1;
}

// ============================================================================
// R4 - CHAMAR PRÓXIMO
// ============================================================================

PacienteAtendido *chamar_proximo() {
    if (fila == NULL || fila->tamanho == 0) return NULL;

    while (fila->tamanho > 0) {
        if (fila->dados[0].ativo != 1) {
            fila->dados[0] = fila->dados[fila->tamanho - 1];
            fila->tamanho--;
            if (fila->tamanho > 0) descer_heap(fila, 0);
            continue;
        }

        PacienteAtendido *resultado = malloc(sizeof(PacienteAtendido));
        if (resultado == NULL) return NULL;

        if (totalAtendidos == atendidosCap) {
            int novaCap = (atendidosCap == 0) ? 16 : atendidosCap * 2;
            PacienteAtendido *temp = realloc(atendidos, novaCap * sizeof(PacienteAtendido));
            if (temp == NULL) {
                free(resultado);
                return NULL;
            }
            atendidos = temp;
            atendidosCap = novaCap;
        }

        ElementoHeap topo = fila->dados[0];
        fila->dados[0] = fila->dados[fila->tamanho - 1];
        fila->tamanho--;
        if (fila->tamanho > 0) descer_heap(fila, 0);

        resultado->paciente = topo.paciente;
        resultado->risco = topo.risco;
        resultado->ordemEntrada = topo.ordemEntrada;
        resultado->ordemAtendimento = relogio_eventos;
        relogio_eventos++;

        Paciente *paciente = buscar_cadastro(topo.paciente.cpf);
        if (paciente != NULL) paciente->ativo_na_fila = 0;

        resultado->paciente.ativo_na_fila = 0;
        fila_ativos--;

        atendidos[totalAtendidos] = *resultado;
        totalAtendidos++;
        return resultado;
    }
    return NULL;
}

// ============================================================================
// R5 - DESISTIR
// ============================================================================

int desistir(const char *cpf) {
    if (fila == NULL || fila->tamanho == 0) return 0;
    Paciente *paciente = buscar_cadastro(cpf);
    if (paciente == NULL || paciente->ativo_na_fila != 1) return 0;

    for (int i = 0; i < fila->tamanho; i++) {
        if (fila->dados[i].ativo == 1 && strcmp(fila->dados[i].paciente.cpf, cpf) == 0) {
            fila->dados[i].ativo = 0;
            paciente->ativo_na_fila = 0;
            fila_ativos--;
            return 1;
        }
    }
    return 0;
}

// ============================================================================
// R6 - TAMANHO DA FILA
// ============================================================================

int tamanho_fila() {
    return fila_ativos;
}

// ============================================================================
// LIBERAR TABELA HASH
// ============================================================================

void liberar_tabela_hash() {
    for (int i = 0; i < HASH_SIZE; i++) {
        HashNode *atual = tabela_hash[i];
        while (atual != NULL) {
            HashNode *temp = atual;
            atual = atual->prox;
            free(temp);
        }
        tabela_hash[i] = NULL;
    }
}

// ============================================================================
// LIBERAR MEMÓRIA
// ============================================================================

void liberar_memoria() {
    free(cadastro);
    if (fila != NULL) {
        free(fila->dados);
        free(fila);
        fila = NULL;
    }
    free(atendidos);
    cadastro = NULL;
    atendidos = NULL;
    liberar_tabela_hash();
}

// ============================================================================
// RESETAR SISTEMA
// ============================================================================

void resetar_sistema() {
    liberar_memoria();
    totalPacientes = 0;
    totalAtendidos = 0;
    atendidosCap = 0;
    fila_ativos = 0;
    relogio_eventos = 0;
}

// ============================================================================
// TIMER DE ALTA RESOLUCAO
// ============================================================================

#ifdef _WIN32
double tempo_agora() {
    LARGE_INTEGER freq, cont;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&cont);
    return (double)cont.QuadPart / (double)freq.QuadPart;
}
#else
double tempo_agora() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}
#endif

// ============================================================================
// TESTE DOS GARGALOS
// ============================================================================

void testar_gargalos(int N) {
    char cpf_temp[15];
    double inicio, fim;
    int repeticoes = 1000;

    resetar_sistema();
    printf("--- Testando FASE 3 (HEAP) para N = %d registros ---\n", N);

    for (int i = 0; i < N; i++) {
        sprintf(cpf_temp, "%011d", i);
        cadastrar(cpf_temp, "Paciente", "01/01/2000");
        int risco = (rand() % 5) + 1;
        dar_entrada(cpf_temp, risco);
    }
    sprintf(cpf_temp, "%011d", N - 1);

    // R2
    inicio = tempo_agora();
    volatile Paciente *resultado_busca = NULL;
    for (int j = 0; j < repeticoes; j++) {
        resultado_busca = buscar_cadastro(cpf_temp);
    }
    (void)resultado_busca;
    fim = tempo_agora();
    printf("Tempo R2 (Busca): %.8f segundos (Media)\n", (fim - inicio) / repeticoes);

    // R4
    inicio = tempo_agora();
    for (int j = 0; j < repeticoes; j++) {
        PacienteAtendido *p = chamar_proximo();
        if (p != NULL) free(p);
    }
    fim = tempo_agora();
    printf("Tempo R4 (Chamada no Heap - O(log N)): %.8f segundos (Media)\n\n", (fim - inicio) / repeticoes);
}

// ============================================================================
// INSERTION SORT (CORRIGIDO PARA ORDENAR POR ESPERA, DECRESCENTE)
// ============================================================================

void insertion_sort(PacienteAtendido vetor[], int tamanho) {
    for (int i = 1; i < tamanho; i++) {
        PacienteAtendido atual = vetor[i];
        int espera_atual = atual.ordemAtendimento - atual.ordemEntrada;

        int j = i - 1;
        while (j >= 0) {
            int espera_j = vetor[j].ordemAtendimento - vetor[j].ordemEntrada;

            // Queremos ordem decrescente, então empurramos para a direita
            // se o elemento anterior tiver esperado MENOS que o atual
            if (espera_j < espera_atual) {
                vetor[j + 1] = vetor[j];
                j--;
            } else {
                break;
            }
        }
        vetor[j + 1] = atual;
    }
}

// ============================================================================
// INTERCALAR (CORRIGIDO PARA ORDENAR POR ESPERA, DECRESCENTE)
// ============================================================================

void intercalar(PacienteAtendido vetor[], PacienteAtendido auxiliar[], int inicio, int meio, int fim) {
    int i = inicio;
    int j = meio + 1;
    int k = inicio;

    while (i <= meio && j <= fim) {
        int espera_i = vetor[i].ordemAtendimento - vetor[i].ordemEntrada;
        int espera_j = vetor[j].ordemAtendimento - vetor[j].ordemEntrada;

        // Decrescente: o maior tempo de espera tem preferência
        if (espera_i >= espera_j) {
            auxiliar[k] = vetor[i];
            i++;
        } else {
            auxiliar[k] = vetor[j];
            j++;
        }
        k++;
    }

    while (i <= meio) {
        auxiliar[k] = vetor[i];
        i++;
        k++;
    }
    while (j <= fim) {
        auxiliar[k] = vetor[j];
        j++;
        k++;
    }

    for (i = inicio; i <= fim; i++) {
        vetor[i] = auxiliar[i];
    }
}

// ============================================================================
// MERGE SORT RECURSIVO
// ============================================================================

void merge_sort_recursivo(PacienteAtendido vetor[], PacienteAtendido auxiliar[], int inicio, int fim) {
    if (inicio >= fim) return;
    int meio = inicio + (fim - inicio) / 2;

    merge_sort_recursivo(vetor, auxiliar, inicio, meio);
    merge_sort_recursivo(vetor, auxiliar, meio + 1, fim);
    intercalar(vetor, auxiliar, inicio, meio, fim);
}

// ============================================================================
// MERGE SORT
// ============================================================================

void merge_sort(PacienteAtendido vetor[], int tamanho) {
    PacienteAtendido *auxiliar = malloc(tamanho * sizeof(PacienteAtendido));
    if (auxiliar == NULL) {
        printf("Erro ao alocar memoria para Merge Sort.\n");
        return;
    }
    merge_sort_recursivo(vetor, auxiliar, 0, tamanho - 1);
    free(auxiliar);
}

// ============================================================================
// GERAR REGISTROS (CORRIGIDO PARA GERAR TEMPOS DE ESPERA VARIADOS)
// ============================================================================

void gerar_registros_atendimento(PacienteAtendido vetor[], int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        memset(&vetor[i], 0, sizeof(PacienteAtendido));
        vetor[i].risco = (rand() % 5) + 1;
        vetor[i].ordemEntrada = i;

        // Simula um tempo de atendimento aleatório maior que a ordem de entrada
        // para gerar tempos de espera diferentes e testar a ordenação corretamente
        vetor[i].ordemAtendimento = i + (rand() % 1000);
    }

    for (int i = tamanho - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        PacienteAtendido temp = vetor[i];
        vetor[i] = vetor[j];
        vetor[j] = temp;
    }
}

// ============================================================================
// TESTE R7
// ============================================================================

void testar_ordenacao_R7(int M) {
    printf("=============================================\n");
    printf("R7 - Ordenacao com M = %d registros\n", M);
    printf("=============================================\n");

    PacienteAtendido *original = malloc(M * sizeof(PacienteAtendido));
    PacienteAtendido *vetorInsertion = malloc(M * sizeof(PacienteAtendido));
    PacienteAtendido *vetorMerge = malloc(M * sizeof(PacienteAtendido));

    if (original == NULL || vetorInsertion == NULL || vetorMerge == NULL) {
        printf("Erro ao alocar memoria.\n");
        free(original);
        free(vetorInsertion);
        free(vetorMerge);
        return;
    }

    gerar_registros_atendimento(original, M);
    memcpy(vetorInsertion, original, M * sizeof(PacienteAtendido));
    memcpy(vetorMerge, original, M * sizeof(PacienteAtendido));

    double inicio = tempo_agora();
    insertion_sort(vetorInsertion, M);
    double fim = tempo_agora();
    double tempoInsertion = fim - inicio;

    inicio = tempo_agora();
    merge_sort(vetorMerge, M);
    fim = tempo_agora();
    double tempoMerge = fim - inicio;

    printf("Insertion Sort: %.6f segundos\n", tempoInsertion);
    printf("Merge Sort:     %.6f segundos\n\n", tempoMerge);

    free(original);
    free(vetorInsertion);
    free(vetorMerge);
}

// ============================================================================
// MAIN
// ============================================================================

int main() {
    srand(42);

    printf("===================================================\n");
    printf("       BENCHMARK DO PROJETO - FASE 3 (HEAP)        \n");
    printf("===================================================\n\n");

    testar_gargalos(10000);
    testar_gargalos(100000);

    testar_ordenacao_R7(10000);
    testar_ordenacao_R7(100000);

    resetar_sistema();
    return 0;
}