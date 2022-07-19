// Esqueleto de um simulador de memória virtual.
// Feito para a disciplina DCC065 - Sistemas Operacionais (UFMG)
// Baseado no código python:
// https://github.com/thiagomanel/labSO/tree/master/lab5

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Campos da tabela de páginas
#define PT_FIELDS 6           // 4 campos na tabela
#define PT_FRAMEID 0          // Endereço da memória física
#define PT_MAPPED 1           // Endereço presente na tabela
#define PT_DIRTY 2            // Página dirty
#define PT_REFERENCE_BIT 3    // Bit de referencia
#define PT_REFERENCE_MODE 4   // Tipo de acesso, converter para char
#define PT_AGING_COUNTER 5    // Contador para aging

// Tipos de acesso
#define READ 'r'
#define WRITE 'w'

// Define a função que simula o algoritmo da política de subst.
typedef int (*eviction_f)(int8_t**, int, int, int, int, int);

typedef struct {
    char *name;
    void *function;
} paging_policy_t;

// Codifique as reposições a partir daqui!
// Cada método abaixo retorna uma página para ser trocada. Note também
// que cada algoritmo recebe:
// - A tabela de páginas
// - O tamanho da mesma
// - A última página acessada
// - A primeira modura acessada (para fifo)
// - O número de molduras
// - Se a última instrução gerou um ciclo de clock
//
// Adicione mais parâmetros caso ache necessário

int fifo(int8_t** page_table, int num_pages, int prev_page,
         int fifo_frm, int num_frames, int clock) {

	int i, aux;                                                                        // contador p/ for e um aux p/ guardar o indice do fifo p/ evitar bugs

    for(i=0; i<num_pages; i++){
    	if(page_table[i][PT_FRAMEID] == fifo_frm && page_table[i][PT_MAPPED] == 1){	   // checa se o frameid e igual ao fifo-frm e se esta mapeado
    		return i;                                                                  // retorna o valor correspondente
    	}else if(page_table[i][PT_MAPPED] == 1){
            aux = i;
    	}
	}
	return aux;
}

int second_chance(int8_t** page_table, int num_pages, int prev_page,
                  int fifo_frm, int num_frames, int clock) {

    int i, j, aux;                                                                     // contadores p/ for e um aux p/ guardar o indice do fifo p/ evitar bugs

    for(j = 0; j < num_pages; j++){
        if(page_table[j][PT_MAPPED] == 1 && page_table[j][PT_FRAMEID] == fifo_frm){   // busca o primeiro a sair de acordo com o fifo
            aux = j;                                                                  // guarda a posição e sai do laco
            break;
        }
    }

    for(i = 0; i < num_pages; i++){
        if(page_table[(aux+i) % num_pages][PT_REFERENCE_BIT] == 0 && page_table[i][PT_MAPPED] == 1){ // comeca a procurar o primeiro bit R = 0 a partir do fifo
            page_table[i][PT_REFERENCE_BIT] = 1;                                                     // seta o bit R como 1 e o retorna

            return i;
        }
        page_table[i][PT_REFERENCE_BIT] = 0;                                                         // caso tenha recebido a segunda chance seu bit R vira 0
    }

    page_table[aux][PT_REFERENCE_BIT] = 1;
	return aux;
}

int nru(int8_t** page_table, int num_pages, int prev_page,
        int fifo_frm, int num_frames, int clock) {

    int i, validsIndex[num_frames], indexBitR[num_frames], indexBitM[num_frames], countBitROne = 0, countBitMOne = 0, countIndex = 0, aux;
//   cont, indices mapeador       , indices com bit R = 1, indices com bit M = 1, contador de bit R, contador de bit M, contador de indices

    for(i = 0; i < num_pages; i++){                             // primeiro mapearemos todos os valores e seus bits
        if(page_table[i][PT_MAPPED] == 1){                      // bit mapeado?
                validsIndex[countIndex] = i;                        // guarda o indice
                countIndex++;                                       // incrementa o contador de indices
                if(page_table[i][PT_REFERENCE_BIT] == 1){           // bit R = 1?
                    indexBitR[countBitROne] = i;                        // guarde o indice
                    countBitROne++;                                     // incrementa contador bit R
                }

                if(page_table[i][PT_DIRTY] == 1){                   // bit M = 1?
                    indexBitM[countBitMOne] = i;                        // guarde o indice
                    countBitMOne++;                                     // incrementa contador bit M
                }
        }
    }

    if((countBitROne == 0 && countBitMOne == 0) || (countBitROne == num_frames && countBitMOne == num_frames)){ // todos indices classe 1 ou 4 ?
        aux = rand() % num_frames;                                                                              // gera indice aleatorio
        page_table[validsIndex[aux]][PT_REFERENCE_BIT] = 1;                                                     // e de acordo com o vetor de indices remove
        return validsIndex[aux];
    }

    if(countBitROne == 0) {                                                         // todos os bit R = 0?
        for(i = 0; i < num_frames; i++){                                                // checa se algum tem bit M = 0
            if(page_table[indexBitM[i]][PT_DIRTY] == 0){                                // bit M = 0?
                page_table[indexBitM[i]][PT_REFERENCE_BIT] = 1;                             // caso haja ele sai
                return indexBitM[i];
            }

        }
        aux = rand() % num_frames;                                                      // caso não haja são todos da mesma classe e um aleatorio sai
        page_table[validsIndex[aux]][PT_REFERENCE_BIT] = 1;
        return validsIndex[aux];
    }

    if(countBitMOne == 0){                                                           // todos os bit M = 0?
        for(i = 0; i < num_frames; i++){                                                // checa se algum tem bit R = 0
            if(page_table[indexBitR[i]][PT_REFERENCE_BIT] == 0){                        // bit R = 0?
                page_table[indexBitR[i]][PT_REFERENCE_BIT] = 1;                             // caso haja ele sai
                return indexBitR[i];
            }
        }
        aux = rand() % num_frames;                                                  // caso não haja são todos da mesma classe e um aleatorio sai
        page_table[validsIndex[aux]][PT_REFERENCE_BIT] = 1;
        return validsIndex[aux];
    }

    // daqui p/ baixo ele verificará alguns caso para que não haja bugs

    // primeiro verifica se tem algum valor com R = 0 e M = 0 e o procura

    // segundo procura se tem algum valor com bit R = 0 e procura o melhor caso

    // terceiro procura se tem algum valor com bit M = 0 e o procura o melhor caso
    if(countBitROne < num_frames && countBitMOne < num_frames){
         for(i = 0; i < num_frames; i++){
            if(page_table[validsIndex[i]][PT_REFERENCE_BIT] == 0 && page_table[validsIndex[i]][PT_DIRTY] == 0){
                page_table[validsIndex[i]][PT_REFERENCE_BIT] = 1;
                return validsIndex[i];
            }
         }
    }

    if(countBitROne < num_frames){
         for(i = 0; i < num_frames; i++){
            if(page_table[validsIndex[i]][PT_REFERENCE_BIT] == 0){
                page_table[validsIndex[i]][PT_REFERENCE_BIT] = 1;
                return validsIndex[i];
            }
         }
    }

    if(countBitMOne < num_frames){
         for(i = 0; i < num_frames; i++){
            if(page_table[validsIndex[i]][PT_DIRTY] == 0){
                page_table[validsIndex[i]][PT_REFERENCE_BIT] = 1;
                return validsIndex[i];
            }
         }
    }

    return -1;
}

int aging(int8_t** page_table, int num_pages, int prev_page,
          int fifo_frm, int num_frames, int clock) {

    int i, minAging = 9999, minAgingIndex = 0;                  // variavel para armazenar o indice e o valor do menor contador aging

    for(i = 0; i < num_pages; i++){                             // percorrendo o vetor
        if(page_table[i][PT_MAPPED] == 1){                      // esta mapeado?
            if(page_table[i][PT_AGING_COUNTER] < minAging){         // contador aging menor que o já registrado?
                minAging = page_table[i][PT_AGING_COUNTER];             // armazena o valor e o indice
                minAgingIndex = i;
            }
        }
    }

    return minAgingIndex;
}

int mfu(int8_t** page_table, int num_pages, int prev_page,
          int fifo_frm, int num_frames, int clock) {

    int i, maxAging = 0, minAgingIndex = 0;                     // variavel para armazenar o indice e o valor do maior contador aging

    for(i = 0; i < num_pages; i++){                             // percorrendo o vetor
        if(page_table[i][PT_MAPPED] == 1){                      // esta mapeado?
            if(page_table[i][PT_AGING_COUNTER] > maxAging){         // contador aging maior que o já registrado?
                maxAging = page_table[i][PT_AGING_COUNTER];         // armazena o valor e o indice
                minAgingIndex = i;
            }
        }
    }

    return minAgingIndex;
}

int random_page(int8_t** page_table, int num_pages, int prev_page,
                int fifo_frm, int num_frames, int clock) {
    int page = rand() % num_pages;
    while (page_table[page][PT_MAPPED] == 0) // Encontra página mapeada
        page = rand() % num_pages;
    return page;
}

// Simulador a partir daqui

int find_next_frame(int *physical_memory, int *num_free_frames,
                    int num_frames, int *prev_free) {
    if (*num_free_frames == 0) {
        return -1;
    }

    // Procura por um frame livre de forma circula na memória.
    // Não é muito eficiente, mas fazer um hash em C seria mais custoso.
    do {
        *prev_free = (*prev_free + 1) % num_frames;
    } while (physical_memory[*prev_free] == 1);

    return *prev_free;
}



// funcao para decrementar o valor do aging dos endereços nao referenciados

void modifyAgingNotUsed(int8_t **page_table, int num_pages, int virt_addrUsed){
    int i;

    for(i = 0; i < num_pages; i++){
        if(page_table[i][PT_MAPPED] == 1 && i != virt_addrUsed){ // decrementa todos menos o valor acertado
            if(page_table[i][PT_AGING_COUNTER] > 0)              // verifica se ja e zero
                page_table[i][PT_AGING_COUNTER] -= 2;
        }
    }
}




int simulate(int8_t **page_table, int num_pages, int *prev_page, int *fifo_frm,
             int *physical_memory, int *num_free_frames, int num_frames,
             int *prev_free, int virt_addr, char access_type,
             eviction_f evict, int clock, char algorithm[]) {
    if (virt_addr >= num_pages || virt_addr < 0) {
        printf("Invalid access \n");
        exit(1);
    }

    // verificacao aging e MFU
    if (page_table[virt_addr][PT_MAPPED] == 1) {
        page_table[virt_addr][PT_REFERENCE_BIT] = 1;
        if(strcmp(algorithm, "aging") == 0){ // se o algoritmo for aging incrementa o contador em 4 do enderço e decrementa em 2 os demais
            page_table[virt_addr][PT_AGING_COUNTER] += 4;
            modifyAgingNotUsed(page_table, num_pages, virt_addr);
        }else if(strcmp(algorithm, "mfu") == 0){ // se o algoritmo for MFU incrementa o valor do endereço
            page_table[virt_addr][PT_AGING_COUNTER] += 1;
        }
        return 0; // Not Page Fault!
    }

    int next_frame_addr;
    if ((*num_free_frames) > 0) { // Ainda temos memória física livre!
        next_frame_addr = find_next_frame(physical_memory, num_free_frames,
                                          num_frames, prev_free);
        if (*fifo_frm == -1)
            *fifo_frm = next_frame_addr;
        *num_free_frames = *num_free_frames - 1;
    } else { // Precisamos liberar a memória!
        assert(*num_free_frames == 0);
        int to_free = evict(page_table, num_pages, *prev_page, *fifo_frm,
                            num_frames, clock);
        assert(to_free >= 0);
        assert(to_free < num_pages);
        assert(page_table[to_free][PT_MAPPED] != 0);

        next_frame_addr = page_table[to_free][PT_FRAMEID];
        *fifo_frm = (*fifo_frm + 1) % num_frames;
        // Libera pagina antiga
        page_table[to_free][PT_FRAMEID] = -1;
        page_table[to_free][PT_MAPPED] = 0;
        page_table[to_free][PT_DIRTY] = 0;
        page_table[to_free][PT_REFERENCE_BIT] = 0;
        page_table[to_free][PT_REFERENCE_MODE] = 0;
        page_table[to_free][PT_AGING_COUNTER] = 0;
    }

    // Coloca endereço físico na tabela de páginas!
    int8_t *page_table_data = page_table[virt_addr];
    page_table_data[PT_FRAMEID] = next_frame_addr;



    // adicionando ao contador aging quando o endereço entra na memoria
    if(strcmp(algorithm, "aging") == 0){
        page_table[virt_addr][PT_AGING_COUNTER] += 4;

    }else if(strcmp(algorithm, "mfu") == 0){
        page_table[virt_addr][PT_AGING_COUNTER] += 1;
    }



    page_table_data[PT_MAPPED] = 1;
    if (access_type == WRITE) {
        page_table_data[PT_DIRTY] = 1;
    }
    page_table_data[PT_REFERENCE_BIT] = 1;
    page_table_data[PT_REFERENCE_MODE] = (int8_t) access_type;
    *prev_page = virt_addr;

    if (clock == 1) {
        for (int i = 0; i < num_pages; i++)
            page_table[i][PT_REFERENCE_BIT] = 0;
    }

    return 1; // Page Fault!
}

void run(int8_t **page_table, int num_pages, int *prev_page, int *fifo_frm,
         int *physical_memory, int *num_free_frames, int num_frames,
         int *prev_free, eviction_f evict, int clock_freq, char algorithm[]) {
    int virt_addr;
    char access_type;
    int i = 0;
    int clock = 0;
    int faults = 0;
    while (scanf("%d", &virt_addr) == 1) {
        getchar();
        scanf("%c", &access_type);
        clock = ((i+1) % clock_freq) == 0;
        faults += simulate(page_table, num_pages, prev_page, fifo_frm,
                           physical_memory, num_free_frames, num_frames, prev_free,
                           virt_addr, access_type, evict, clock, algorithm);
        i++;
    }
    printf("%d\n", faults);
}

int parse(char *opt) {
    char* remainder;
    int return_val = strtol(opt, &remainder, 10);
    if (strcmp(remainder, opt) == 0) {
        printf("Error parsing: %s\n", opt);
        exit(1);
    }
    return return_val;
}

void read_header(int *num_pages, int *num_frames) {
    scanf("%d %d\n", num_pages, num_frames);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Usage %s <algorithm> <clock_freq>\n", argv[0]);
        exit(1);
    }

    char *algorithm = argv[1];
    int clock_freq = parse(argv[2]);
    int num_pages;
    int num_frames;
    read_header(&num_pages, &num_frames);

    // Aponta para cada função que realmente roda a política de parse
    paging_policy_t policies[] = {
            {"fifo", *fifo},
            {"second_chance", *second_chance},
            {"nru", *nru},
            {"aging", *aging},
            {"mfu", *mfu},
            {"random", *random_page}
    };

    int n_policies = sizeof(policies) / sizeof(policies[0]);
    eviction_f evict = NULL;
    for (int i = 0; i < n_policies; i++) {
        if (strcmp(policies[i].name, algorithm) == 0) {
            evict = policies[i].function;
            break;
        }
    }

    if (evict == NULL) {
        printf("Please pass a valid paging algorithm.\n");
        exit(1);
    }

    // Aloca tabela de páginas
    int8_t **page_table = (int8_t **) malloc(num_pages * sizeof(int8_t*));
    for (int i = 0; i < num_pages; i++) {
        page_table[i] = (int8_t *) malloc(PT_FIELDS * sizeof(int8_t));
        page_table[i][PT_FRAMEID] = -1;
        page_table[i][PT_MAPPED] = 0;
        page_table[i][PT_DIRTY] = 0;
        page_table[i][PT_REFERENCE_BIT] = 0;
        page_table[i][PT_REFERENCE_MODE] = 0;
        page_table[i][PT_AGING_COUNTER] = 0;
    }

    // Memória Real é apenas uma tabela de bits (na verdade uso ints) indicando
    // quais frames/molduras estão livre. 0 == livre!
    int *physical_memory = (int *) malloc(num_frames * sizeof(int));
    for (int i = 0; i < num_frames; i++) {
        physical_memory[i] = 0;
    }
    int num_free_frames = num_frames;
    int prev_free = -1;
    int prev_page = -1;
    int fifo_frm = -1;

    // Roda o simulador
    srand(time(NULL));
    run(page_table, num_pages, &prev_page, &fifo_frm, physical_memory,
        &num_free_frames, num_frames, &prev_free, evict, clock_freq, algorithm);

    // Liberando os mallocs
    for (int i = 0; i < num_pages; i++) {
        free(page_table[i]);
    }
    free(page_table);
    free(physical_memory);
}
