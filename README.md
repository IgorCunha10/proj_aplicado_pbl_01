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

https://file.kiwi/41ae6440#Y6QqGupPteNa4ZYvT2ZuYw
---

Projeto acadêmico desenvolvido para estudo de:

* Estruturas de Dados
* Filas de Prioridade
* Tabelas Hash
* Algoritmos de Ordenação
* Análise de Complexidade

--- 

