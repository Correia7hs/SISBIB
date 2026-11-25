#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define ARQUIVO_USUARIO "usuario.csv"
#define ARQUIVO_LIVRO "livros.csv"
#define ARQUIVO_AUTOR "autor.csv"
#define ARQUIVO_EMPRESTIMO "emprestimo.csv"
#define ARQUIVO_TEMP "temp.csv"
#define MAX_NOME 80

const char* const S[] ={"ATIVO", "INATIVO", "SUSPENSO"};

typedef struct{
	char autor01[MAX_NOME],autor02[MAX_NOME],autor03[MAX_NOME];
}Nomeautores; //struct para usar na função relacionar_id_autor 

typedef struct{
	char nome_usuario[MAX_NOME];
    int id_usuario;
	char status[20];
}Usuario;


typedef struct{
	char nome_livro[MAX_NOME];
	int id_livro;
    int id_autores[3];
    int estoque;
}Livro;

typedef struct{
	char nome_autor[MAX_NOME];
	int id_autor;
}Autor;

typedef struct{
    int id_emprestimo;
    int id_usuario;
    int id_livro;
    char status[12];
}Emprestimo;

void pausar();
int proximo_id(const char *arquivo_c);
void garantir_arquivos();
void limpar_buffer();
void limpar_tela();
void cadastrar_usuario();
void cadastrar_autor();
void cadastrar_livro();
Nomeautores relacionar_id_autor(int autor1, int autor2, int autor3);
void listar_usuarios();
void listar_autores();
void listar_livro();
void atualizar_usuario();
void atualizar_autor();
void atualizar_livro();
void excluir_usuario();
void excluir_autor();
void excluir_livro();
void cadastrar_emprestimo();
void alterar_emprestimo(int excluir);
void relatorio_emprestimos();
void gerenciar_estoque();
int buscar_estoque(int id_livro, int *total, int *disponiveis);
int salvar_estoque(int id_livro, int total);
static int contar_emprestimos_ativos(int id_livro);
void garantir_arquivo_emprestimo();
int ler_livro_linha(const char *linha, Livro *l);
int ler_emprestimo_linha(const char *linha, Emprestimo *e);

void pausar(){
    printf("\nPressione Enter para continuar...");
    getchar();
}

int proximo_id(const char *arquivo_c) {
    FILE *arquivo = fopen(arquivo_c, "r");
    if (arquivo == NULL) {
        return 100; // arquivo ainda não existe
    }

    char linha[300];
    int id;
    int maior = 99;

    if (fgets(linha, sizeof(linha), arquivo) == NULL) {
        fclose(arquivo);
        return 100; // arquivo vazio
    }

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (sscanf(linha, "%d;", &id) == 1 && id > maior) {
            maior = id;
        }
    }

    fclose(arquivo);
    return maior + 1;
}

void garantir_arquivos(){
  FILE *usuario = fopen(ARQUIVO_USUARIO, "r");
  FILE *livro = fopen(ARQUIVO_LIVRO, "r");
  FILE *autor = fopen(ARQUIVO_AUTOR, "r");

 if(usuario == NULL || livro == NULL || autor == NULL){
	 if(usuario == NULL){
         FILE *usuario = fopen(ARQUIVO_USUARIO, "w");
		 if(usuario != NULL){
             fprintf(usuario, "%s;%s;%s\n", "id", "nome", "status");
		     fclose(usuario);
		    }
	    }
	 if(livro == NULL){
		 FILE *livro = fopen(ARQUIVO_LIVRO, "w");
		 if(livro != NULL){
			 fprintf(livro, "%s;%s;%s;%s\n", "id", "titulo", "idautores", "estoque");
			 fclose(livro);
		    }
	    }
     if(autor == NULL){
	     FILE *autor = fopen(ARQUIVO_AUTOR, "w");
		 if(autor != NULL){
			 fprintf(autor, "%s;%s\n", "id", "nome");
			 fclose(autor);
		    }

    	}
	
    }

 if(usuario != NULL || livro != NULL || autor != NULL){
	   if(usuario != NULL){
	     fclose(usuario);
        }
	   if(livro != NULL){
         fclose(livro);
	    }
	   if(autor != NULL){
	     fclose(autor);	
	    }

    }

 garantir_arquivo_emprestimo();

}

void garantir_arquivo_emprestimo(){
    FILE *emprestimo = fopen(ARQUIVO_EMPRESTIMO, "a+");
    if(emprestimo == NULL) return;
    fseek(emprestimo, 0, SEEK_END);
    if(ftell(emprestimo) == 0) fputs("id;id_usuario;id_livro;status\n", emprestimo);
    fclose(emprestimo);
}

void limpar_buffer(){
	int c;
	while((c = getchar()) != '\n' && c != EOF ){}
}

void limpar_tela(void) {
    printf("\033[2J\033[H");  /* limpa a tela e posiciona o cursor no topo */
}



void cadastrar_usuario(){
 Usuario u;
 FILE *usuario = fopen(ARQUIVO_USUARIO, "a"); //modo append para adicionar informações 

 if(usuario == NULL){ 
     printf("Erro ao abir %s", ARQUIVO_USUARIO);
	 return;
    }
 limpar_buffer(); 
 printf("Nome do usuário\n");
 fgets(u.nome_usuario, MAX_NOME, stdin);
 u.nome_usuario[strcspn(u.nome_usuario, "\n")] = '\0';

 strcpy(u.status, S[0]); //status padrão

 u.id_usuario = proximo_id(ARQUIVO_USUARIO);
 
 fprintf(usuario, "%d;%s;%s\n", u.id_usuario, u.nome_usuario, u.status);
 fclose(usuario);

}


void cadastrar_autor(){
 Autor a;
 FILE *autor = fopen(ARQUIVO_AUTOR, "a");

 if(autor == NULL){ 
     printf("Erro ao abir %s", ARQUIVO_AUTOR);
	 return;
    }
 limpar_buffer();
 printf("Nome do autor\n");
 fgets(a.nome_autor, MAX_NOME, stdin);
 a.nome_autor[strcspn(a.nome_autor, "\n")] = '\0';

 a.id_autor = proximo_id(ARQUIVO_AUTOR);

 fprintf(autor, "%d;%s\n", a.id_autor, a.nome_autor);
 fclose(autor);
}


void cadastrar_livro(){
 Livro l;
 memset(&l, 0, sizeof(l));
 FILE *livro = fopen(ARQUIVO_LIVRO, "a");
 int opcao, quantidade; //usado para guardar as opções 1 - Sim , 2 - Não
 int i; //usada para percorrer o vetor do id de autores

 if(livro == NULL){ 
     printf("Erro ao abir %s", ARQUIVO_LIVRO);
	 return;
    }

 printf("Título\n");
 fgets(l.nome_livro, MAX_NOME, stdin);
 l.nome_livro[strcspn(l.nome_livro, "\n")] = '\0';

 do{
     listar_autores();
     printf("Encontrou seu(s) autor(es)?\n"
           "1 - Sim!\n"
           "2 - Não!\n");
     scanf("%d", &opcao);

     if(opcao != 1 && opcao != 2){
         printf("Digite uma opção válida!\n"
               "Aguarde...");
         sleep(2);
         limpar_tela();
        }
     else if(opcao == 2){
		 limpar_tela();
         cadastrar_autor();
        }
    }while(opcao != 1);  

 for(i = 0; i < 3; i++){
     printf("Digite o código do %dº autor\n", i + 1);
     scanf(" %d", &l.id_autores[i]);
	 limpar_buffer();

     if(i == 2){
         limpar_tela();
         break;
        }

     do{
         printf("Deseja adicionar mais algum autor?\n"
               "1 - Sim!\n"
               "2 - Não!\n");
         scanf("%d", &opcao);
         if(opcao != 1 && opcao != 2){
             printf("Digite uma opção válida!\n"
                    "Aguarde...");
             sleep(2);
             limpar_tela();
            }
        }while(opcao != 1 && opcao != 2);

        limpar_tela();
        if(opcao == 2){ 
		 limpar_buffer();
         break;		
		} 
		 
    }


 do{
     printf("Quantidade inicial de exemplares no estoque\n");
     scanf("%d", &quantidade);
     if(quantidade < 0) printf("A quantidade não pode ser negativa.\n");
 }while(quantidade < 0);

 l.id_livro = proximo_id(ARQUIVO_LIVRO);

 l.estoque = quantidade;
 fprintf(livro, "%d;%s;%d|%d|%d;%d\n", l.id_livro, l.nome_livro,
         l.id_autores[0], l.id_autores[1], l.id_autores[2], l.estoque);
 fclose(livro);

}
 
/*Função para relacionar o autor com seu código*/

Nomeautores relacionar_id_autor(int autor1, int autor2, int autor3){ 
 Autor a;
 Nomeautores n;
 memset(&n, 0, sizeof(n));
 char linha [300];
 FILE *autor = fopen(ARQUIVO_AUTOR, "r");

 if(autor == NULL){
	 printf("Erro ao abrir %s",ARQUIVO_AUTOR);
	 return n;
    }
	
 fgets(linha, sizeof(linha),autor);
 
 while(fscanf(autor, "%d;%79[^\n]\n", &a.id_autor, a.nome_autor) == 2){
     if(autor1 == a.id_autor){
		 strcpy(n.autor01, a.nome_autor);
	    }
	 else if(autor2 == a.id_autor){
		 strcpy(n.autor02, a.nome_autor);
	    }
	 else if(autor3 == a.id_autor){
		 strcpy(n.autor03, a.nome_autor);
	    }
	}

 fclose(autor);
 return n;
}

/*Funções para listar os objetos*/

void listar_usuarios(){
 Usuario u;
 char linha [300];
 FILE *usuario = fopen(ARQUIVO_USUARIO, "r");


 if(usuario == NULL){
 	 printf("Erro ao abrir o arquivo %s",ARQUIVO_USUARIO);
	 return;
    }

 fgets(linha, sizeof(linha), usuario); //pula o cabeçalho

 printf("%-6s | %s\n", "ID", "NOME");
 printf("-------|----------------------------------------\n");

 while(fscanf(usuario, "%d;%79[^;];%[^\n]\n", &u.id_usuario, u.nome_usuario, u.status) == 3){
	 printf("%-6d | %s\n", u.id_usuario, u.nome_usuario);
     printf("       | Status: %s\n\n", u.status);
    }
 pausar();
 fclose(usuario);
}

void listar_autores(){
 Autor a;
 char linha[300];
 FILE *autor = fopen(ARQUIVO_AUTOR, "r");


 if(autor == NULL){
	 printf("Erro ao abrir %s",ARQUIVO_AUTOR);
	 return;
    }
 
 fgets(linha, sizeof(linha), autor); // Pula o cabeçalho

 printf("|%-10s| %-29s|\n", "ID", "NOME");
 printf("|----------|------------------------------|\n");

 while( fscanf(autor, "%d;%79[^\n]\n", &a.id_autor, a.nome_autor) == 2){
	 printf("|%-10d|%-29s\n", a.id_autor,a.nome_autor);


    }
    fclose(autor);
    pausar();
}

void listar_livro(){
 Livro l;
 Nomeautores n;
 memset(&l, 0,sizeof(l)); //Todos os campos da struct são inicializados com o caracter '\0' 
 memset(&n, 0, sizeof(n)); 
 char linha[300];
 int total, disponiveis;

 FILE *livro = fopen(ARQUIVO_LIVRO,"r");

 limpar_buffer();

 if(livro == NULL){
	 printf("Erro ao abrir %s", ARQUIVO_LIVRO);
	 return;
    }

 fgets(linha, sizeof(linha), livro); //pula o cabeçalho

    printf("%-6s | %s\n", "ID", "TÍTULO");
    printf("-------|----------------------------------------\n");

 while(fgets(linha, sizeof(linha), livro) != NULL){
     if(!ler_livro_linha(linha, &l)) continue;
     n = relacionar_id_autor(l.id_autores[0],l.id_autores[1],l.id_autores[2]);
	 if(n.autor01[0] == '\0') strcpy(n.autor01, "Vazio");
	 if(n.autor02[0] == '\0') strcpy(n.autor02, "Vazio");
	 if(n.autor03[0] == '\0') strcpy(n.autor03, "Vazio"); 
	 printf("%-6d | %s\n", l.id_livro, l.nome_livro);
     printf("       | Autores: %s, %s, %s\n\n", n.autor01, n.autor02, n.autor03);
	 buscar_estoque(l.id_livro, &total, &disponiveis);
	 printf("       | Estoque: %d exemplar(es), %d disponível(is)\n\n", total, disponiveis);
	 
    }

 pausar();
 fclose(livro);

}

/*Funçõe para atualizar os objetos*/

void atualizar_usuario(){
   Usuario u;
   FILE *usuario = fopen(ARQUIVO_USUARIO, "r");
   FILE *temp = fopen(ARQUIVO_TEMP, "w");
   char linha[300];
   int id, opcao, atualizou = 0, encontrou = 0;

   if(usuario == NULL){
	 printf("Erro ao abrir %s",ARQUIVO_USUARIO);
	 return;
   }
   if(temp == NULL){
	 printf("Erro ao abrir %s",ARQUIVO_TEMP);
	 return;
   }


   fgets(linha, sizeof(linha), usuario);//pula o cabeçallho
   fputs(linha, temp); //pega o cabeçalho de linha e coloca no arquivo temporário

   printf("Digite o id do usuário\n");
   scanf(" %d",&id);
    
  while(fscanf(usuario, "%d;%79[^;];%[^\n]\n", &u.id_usuario, u.nome_usuario, u.status) == 3){
     if(id == u.id_usuario){
		 encontrou = 1;
         printf("-------------------USUÁRIO ENCONTRADO----------------\n");
         printf("|%-10s | %-28s | %-10s |\n", "ID", "NOME", "STATUS");
         printf("|-----------|------------------------------|------------|\n");
		 printf("|%-10d | %-28s | %-10s |\n",u.id_usuario, u.nome_usuario, u.status);
	     do{
		     printf("Deseja mudar alguma informação?\n"
			    "1 - Sim\n"
			    "2 - Não\n");
		     scanf(" %d", &opcao);

		     if(opcao != 1 && opcao != 2){ 
			     printf("Digite uma opção válida!\n"
			            "Aguarde...");
				 sleep(2);  		
			     limpar_tela();
		    	}
            }while(opcao != 1 && opcao != 2);
		  if(opcao == 1){
		     limpar_tela();
			 limpar_buffer();
		     printf("Insira o novo nome\n");
		     fgets(u.nome_usuario, MAX_NOME, stdin);
		     u.nome_usuario[strcspn(u.nome_usuario, "\n")] = '\0';
             do{
		         printf("Insira o novo status\n"
				        "1 - Ativo\n"
					    "2 - Inativo\n"
					    "3 Suspenso\n");
			     scanf(" %d",&opcao);
			     if(opcao == 1)strcpy(u.status, S[0]);
			     if(opcao == 2)strcpy(u.status, S[1]);
			     if(opcao == 3)strcpy(u.status, S[2]);
				 if(opcao < 1 || opcao > 3){
					 printf("Digite uma opção válida!");
				     sleep(2);
					 limpar_tela();
					} 
				}while(opcao < 1 || opcao > 3);
		     

			 printf("-------------------USUÁRIO ATUALIZADO----------------\n");
             printf("|%-10s | %-28s | %-10s |\n", "ID", "NOME", "STATUS");
             printf("|-----------|------------------------------|------------|\n");
	         printf("|%-10d | %-28s | %-10s |\n",u.id_usuario, u.nome_usuario, u.status);
             
			 pausar();
			 fprintf(temp, "%d;%s;%s\n", u.id_usuario, u.nome_usuario, u.status );

			 atualizou = 1;
            }			 
		  else if(opcao == 2) fprintf(temp, "%d;%s;%s\n", u.id_usuario, u.nome_usuario, u.status );
		}
		else fprintf(temp, "%d;%s;%s\n", u.id_usuario, u.nome_usuario, u.status );
    }

  fclose(usuario);
  fclose(temp);

  if(encontrou){
     if(atualizou){
	     remove(ARQUIVO_USUARIO);
	     rename(ARQUIVO_TEMP,ARQUIVO_USUARIO);
        }
     else remove(ARQUIVO_TEMP);
    }
	else{
	 remove(ARQUIVO_TEMP);
     printf("Usuário não encontrado!");
	}
	   
}

void atualizar_autor(){
 Autor a;
 FILE *autor = fopen(ARQUIVO_AUTOR, "r");
 FILE *temp = fopen(ARQUIVO_TEMP, "w");
 char linha[300];
 int id, opcao, encontrou = 0, atualizou = 0;

 if(autor == NULL){
	 printf("Erro ao abrir %s",ARQUIVO_AUTOR);
	 return;
    }
 if(temp == NULL){
	 printf("Erro ao abrir %s",ARQUIVO_TEMP);
	 return;
    }
 
 fgets(linha, sizeof(linha), autor); //pula o cabeçalho
 fputs(linha, temp); //pega o cabeçalho de linha e coloca no arquivo temporário

 printf("Digite o id do autor\n");
 scanf(" %d",&id);

 while(fscanf(autor, "%d;%79[^\n]\n", &a.id_autor, a.nome_autor) == 2){
     if(id == a.id_autor ){
		 encontrou = 1;
		 printf("--------------AUTOR ENCONTRADO------------\n");
         printf("|%-10s | %-29s|\n", "ID", "NOME");
         printf("|-----------|------------------------------|\n");
		 printf("|%-10d | %-29s|\n",a.id_autor, a.nome_autor);
         do{
		     printf("Deseja mudar alguma informação?\n"
			    "1 - Sim\n"
			    "2 - Não\n");
		     scanf(" %d", &opcao);

		     if(opcao != 1 && opcao != 2){ 
			     printf("Digite uma opção válida!\n"
			            "Aguarde...");
				 sleep(2);		
			     limpar_tela();
		    	}
            }while(opcao != 1 && opcao != 2);
           if(opcao == 1){
		     limpar_tela();
			 limpar_buffer();
		     printf("Insira o novo nome\n");
		     fgets(a.nome_autor, MAX_NOME, stdin);
		     a.nome_autor[strcspn(a.nome_autor, "\n")] = '\0';

			 printf("--------------AUTOR ATUALIZADO------------\n");
             printf("|%-10s | %-29s|\n", "ID", "NOME");
             printf("|-----------|------------------------------|\n");
		     printf("|%-10d | %-29s|\n",a.id_autor, a.nome_autor);


             pausar();
			 fprintf(temp, "%d;%s\n",a.id_autor, a.nome_autor);

			 atualizou = 1;
            }
			else if(opcao == 2) fprintf(temp, "%d;%s\n",a.id_autor, a.nome_autor);			 
	    }
		else fprintf(temp, "%d;%s\n",a.id_autor, a.nome_autor);
    }

 fclose(autor);
 fclose(temp);
 if(encontrou){
	 if(atualizou){
       remove(ARQUIVO_AUTOR);
	   rename(ARQUIVO_TEMP,ARQUIVO_AUTOR);
	 }
	 else remove(ARQUIVO_TEMP);
    }
 else{
	remove(ARQUIVO_TEMP);
	printf("Autor não encontrado!");
 }


}

void atualizar_livro(){
 Livro l;
 Nomeautores n;
 memset(&l, 0, sizeof(l)); //por desencargo de consciência
 memset(&n, 0, sizeof(n));
 FILE *livro = fopen(ARQUIVO_LIVRO, "r");
 FILE *temp = fopen(ARQUIVO_TEMP, "w");
 char linha[300];
 int id, opcao, encontrou = 0, atualizou = 0, i; 

 if(livro == NULL){
	 printf("Erro ao abrir %s",ARQUIVO_LIVRO);
	 return;
    }
 if(temp == NULL){
	 printf("Erro ao abrir %s",ARQUIVO_TEMP);
	 return;
 }

 fgets(linha, sizeof(linha), livro);
 fputs("id;titulo;idautores;estoque\n", temp);

 printf("Digite o código do livro\n");
 scanf("%d", &id);

	 while(fgets(linha, sizeof(linha), livro) != NULL){
	 if(!ler_livro_linha(linha, &l)) continue;
	if(id == l.id_livro){
	   encontrou = 1;
       n = relacionar_id_autor(l.id_autores[0],l.id_autores[1],l.id_autores[2]);
       printf("%-6s | %s\n", "ID", "TÍTULO");
       printf("-------|----------------------------------------\n");
	   printf("%-6d | %s\n", l.id_livro, l.nome_livro);
       printf("       | Autores: %s, %s, %s\n\n", n.autor01, n.autor02, n.autor03);
	    do{
		 printf("Deseja mudar alguma informação?\n"
			    "1 - Sim\n"
			    "2 - Não\n");
		 scanf(" %d", &opcao);

		 if(opcao != 1 && opcao != 2){ 
		     printf("Digite uma opção válida!\n"
			            "Aguarde...");
			 sleep(2);		
			 limpar_tela();
		    }
        }while(opcao != 1 && opcao != 2);
       if(opcao == 1){
		 l.id_autores[0] = 0; //zera todas posições de autores
		 l.id_autores[1] = 0;
		 l.id_autores[2] = 0;
		 limpar_buffer();
         printf("Selecione a opção que deseja atualizar\n"
                "1 - Título\n"
                "2 - Autores\n");
         scanf("%d", &opcao);
         if(opcao == 1){
             limpar_buffer();        
		     printf("Insira o novo título\n");
		     fgets(l.nome_livro, MAX_NOME, stdin);
             l.nome_livro[strcspn(l.nome_livro, "\n")] = '\0';
            }
         if(opcao == 2){       
		     for(i = 0; i < 3; i++){
			     limpar_tela();
			     listar_autores();
			     printf("Digite o código do %dº autor\n"
                        "Se não o achou digite 0\n", i + 1);
                 scanf(" %d", &l.id_autores[i]);
                 if(l.id_autores[i] == 0){
                     limpar_tela();
                     cadastrar_autor();
                     l.id_autores[i] = proximo_id(ARQUIVO_AUTOR) - 1;l.id_autores[i] = proximo_id(ARQUIVO_AUTOR) - 1;
                    }
                 if(i == 2){
                     limpar_tela();
                     break;
                    }

                 do{
                     printf("Deseja adicionar mais algum autor?\n"
                            "1 - Sim!\n"
                            "2 - Não!\n");
                     scanf(" %d", &opcao);
				     limpar_buffer();
                     if(opcao != 1 && opcao != 2){
                         printf("Digite uma opção válida!\n"
                         "Aguarde...");
                          sleep(2);
                          limpar_tela();
                        }
                    }while(opcao != 1 && opcao != 2);
                 
                 if(opcao == 2){ 
                     limpar_tela(); 
                     break;
                    } 
                }
             
              		 
            
            }
         n = relacionar_id_autor(l.id_autores[0],l.id_autores[1],l.id_autores[2]);
		 if(n.autor01[0] == '\0') strcpy(n.autor01, "Vazio");
	     if(n.autor02[0] == '\0') strcpy(n.autor02, "Vazio");
	     if(n.autor03[0] == '\0') strcpy(n.autor03, "Vazio"); 
         printf("%-6s | %s\n", "ID", "TÍTULO");
         printf("-------|----------------------------------------\n");
	     printf("%-6d | %s\n", l.id_livro, l.nome_livro);
         printf("       | Autores: %s, %s, %s\n\n", n.autor01, n.autor02, n.autor03);
         printf("Informações atualizadas!!");
		 atualizou = 1;
         
		 pausar();
		 fprintf(temp, "%d;%s;%d|%d|%d;%d\n",l.id_livro, l.nome_livro, l.id_autores[0],l.id_autores[1],l.id_autores[2],l.estoque);
		 memset(&n, 0, sizeof(n));   
		}
         
		else if(opcao == 2) fprintf(temp, "%d;%s;%d|%d|%d;%d\n",l.id_livro, l.nome_livro, l.id_autores[0],l.id_autores[1],l.id_autores[2],l.estoque);
		
	}
	else fprintf(temp, "%d;%s;%d|%d|%d;%d\n",l.id_livro, l.nome_livro, l.id_autores[0],l.id_autores[1],l.id_autores[2],l.estoque); 
	
 }


 fclose(livro);
 fclose(temp);

 if(encontrou){
     if(atualizou){
		 remove(ARQUIVO_LIVRO);
		 rename(ARQUIVO_TEMP, ARQUIVO_LIVRO);
	    }
	 else remove(ARQUIVO_TEMP);
    }
 else{
	 printf("Livro não encontrado!");
	 remove(ARQUIVO_TEMP);
    }




}

/*Funções para excluir*/

void excluir_usuario() {
    Usuario u;
    char linha[300];
    int id, encontrou = 0;

    FILE *usuario = fopen(ARQUIVO_USUARIO, "r");
    if (usuario == NULL) {
        printf("Erro ao abrir %s\n", ARQUIVO_USUARIO);
        return;
    }

    FILE *temp = fopen(ARQUIVO_TEMP, "w");
    if (temp == NULL) {
        printf("Erro ao abrir %s\n", ARQUIVO_TEMP);
        fclose(usuario);
        return;
    }

    if (fgets(linha, sizeof(linha), usuario) != NULL) {
        fputs(linha, temp); // copia o cabeçalho
    }

    printf("Digite o ID do usuário que deseja excluir:\n");
    scanf("%d", &id);

    while (fscanf(usuario, "%d;%79[^;];%19[^\n]\n",
                  &u.id_usuario, u.nome_usuario, u.status) == 3) {
        if (u.id_usuario == id) {
            encontrou = 1; // não copia o registro: ele será excluído
        } else {
            fprintf(temp, "%d;%s;%s\n",
                    u.id_usuario, u.nome_usuario, u.status);
        }
    }

    fclose(usuario);
    fclose(temp);

    if (encontrou) {
        remove(ARQUIVO_USUARIO);

        if (rename(ARQUIVO_TEMP, ARQUIVO_USUARIO) == 0) {
            printf("Usuário excluído com sucesso.\n");
        } 
		else {
            printf("Erro ao substituir o arquivo de usuários.\n");
        }
    } 
	else {
        remove(ARQUIVO_TEMP);
        printf("Usuário não encontrado.\n");
    }
}

void excluir_autor() {
    Autor a;
    Livro l;
    char linha[300];
    int id, encontrou = 0, autor_em_uso = 0;

    FILE *autor = fopen(ARQUIVO_AUTOR, "r");
    if (autor == NULL) {
        printf("Erro ao abrir %s\n", ARQUIVO_AUTOR);
        return;
    }

    printf("Digite o ID do autor que deseja excluir:\n");
    scanf("%d", &id);

    // Verifica se algum livro ainda referencia este autor.
    FILE *livro = fopen(ARQUIVO_LIVRO, "r");
    if (livro == NULL) {
        printf("Erro ao abrir %s\n", ARQUIVO_LIVRO);
        fclose(autor);
        return;
    }

    if (fgets(linha, sizeof(linha), livro) != NULL) {
        while (fgets(linha, sizeof(linha), livro) != NULL) {
            if (!ler_livro_linha(linha, &l)) continue;
            if (l.id_autores[0] == id ||
                l.id_autores[1] == id ||
                l.id_autores[2] == id) {
                autor_em_uso = 1;
                break;
            }
        }
    }

    fclose(livro);

    if (autor_em_uso) {
        fclose(autor);
        printf("Não é possível excluir: o autor está relacionado a um livro.\n");
        return;
    }

    FILE *temp = fopen(ARQUIVO_TEMP, "w");
    if (temp == NULL) {
        printf("Erro ao abrir %s\n", ARQUIVO_TEMP);
        fclose(autor);
        return;
    }

    if (fgets(linha, sizeof(linha), autor) != NULL) {
        fputs(linha, temp); // copia o cabeçalho
    }

    while (fscanf(autor, "%d;%79[^\n]\n",
                  &a.id_autor, a.nome_autor) == 2) {
        if (a.id_autor == id) {
            encontrou = 1; // não copia o autor
        } else {
            fprintf(temp, "%d;%s\n", a.id_autor, a.nome_autor);
        }
    }

    fclose(autor);
    fclose(temp);

    if (encontrou) {
        remove(ARQUIVO_AUTOR);

        if (rename(ARQUIVO_TEMP, ARQUIVO_AUTOR) == 0) {
            printf("Autor excluído com sucesso.\n");
        } 
		else {
            printf("Erro ao substituir o arquivo de autores.\n");
        }
    } 
	else {
        remove(ARQUIVO_TEMP);
        printf("Autor não encontrado.\n");
    }
}

void excluir_livro() {
    Livro l;
    char linha[300];
    int id, encontrou = 0;

    FILE *livro = fopen(ARQUIVO_LIVRO, "r");
    if (livro == NULL) {
        printf("Erro ao abrir %s\n", ARQUIVO_LIVRO);
        return;
    }

    FILE *temp = fopen(ARQUIVO_TEMP, "w");
    if (temp == NULL) {
        printf("Erro ao abrir %s\n", ARQUIVO_TEMP);
        fclose(livro);
        return;
    }

    if (fgets(linha, sizeof(linha), livro) != NULL) {
        fputs("id;titulo;idautores;estoque\n", temp);
    }

    printf("Digite o ID do livro que deseja excluir:\n");
    scanf("%d", &id);

    if (contar_emprestimos_ativos(id) > 0) {
        fclose(livro);
        fclose(temp);
        remove(ARQUIVO_TEMP);
        printf("Não é possível excluir: há um empréstimo pendente deste livro.\n");
        return;
    }

    while (fgets(linha, sizeof(linha), livro) != NULL) {
        if (!ler_livro_linha(linha, &l)) continue;
        if (l.id_livro == id) {
            encontrou = 1; // não copia o livro
        } else {
            fprintf(temp, "%d;%s;%d|%d|%d;%d\n",
                    l.id_livro, l.nome_livro,
                    l.id_autores[0], l.id_autores[1],
                    l.id_autores[2], l.estoque);
        }
    }

    fclose(livro);
    fclose(temp);

    if (encontrou) {
        remove(ARQUIVO_LIVRO);

        if (rename(ARQUIVO_TEMP, ARQUIVO_LIVRO) == 0) {
            printf("Livro excluído com sucesso.\n");
        } 
		else {
            printf("Erro ao substituir o arquivo de livros.\n");
        }
    } 
	else {
        remove(ARQUIVO_TEMP);
        printf("Livro não encontrado.\n");
    }
}

int ler_livro_linha(const char *linha, Livro *l){
    int campos;
    memset(l, 0, sizeof(*l));
    campos = sscanf(linha, "%d;%79[^;];%d|%d|%d;%d",
                    &(*l).id_livro, (*l).nome_livro, &(*l).id_autores[0],
                    &(*l).id_autores[1], &(*l).id_autores[2], &(*l).estoque);
    if(campos == 5){
        (*l).estoque = 1;
        return 1;
    }
    return campos == 6;
}

int ler_emprestimo_linha(const char *linha, Emprestimo *e){
    char campo_antigo[11], devolucao_antiga[11];
    int campos = sscanf(linha, "%d;%d;%d;%10[^;];%10[^\r\n]",
                        &(*e).id_emprestimo, &(*e).id_usuario, &(*e).id_livro,
                        campo_antigo, devolucao_antiga);
    if(campos == 5){
        strcpy((*e).status, strcmp(devolucao_antiga, "PENDENTE") == 0 ? "PENDENTE" : "DEVOLVIDO");
        return 1;
    }
    return sscanf(linha, "%d;%d;%d;%11[^;\r\n]", &(*e).id_emprestimo,
                  &(*e).id_usuario, &(*e).id_livro, (*e).status) == 4;
}

static int contar_emprestimos_ativos(int id_livro){
    FILE *emprestimo = fopen(ARQUIVO_EMPRESTIMO, "r");
    char linha[200];
    Emprestimo e;
    int quantidade = 0;
    if(emprestimo == NULL) return 0;
    fgets(linha, sizeof(linha), emprestimo);
    while(fgets(linha, sizeof(linha), emprestimo) != NULL){
        if(ler_emprestimo_linha(linha, &e) && e.id_livro == id_livro &&
           strcmp(e.status, "PENDENTE") == 0)
            quantidade++;
    }
    fclose(emprestimo);
    return quantidade;
}

int buscar_estoque(int id_livro, int *total, int *disponiveis){
    FILE *livro = fopen(ARQUIVO_LIVRO, "r");
    char linha[300];
    Livro l;
    *total = 0;
    *disponiveis = 0;
    if(livro == NULL) return 0;
    fgets(linha, sizeof(linha), livro);
    while(fgets(linha, sizeof(linha), livro) != NULL){
        if(ler_livro_linha(linha, &l) && l.id_livro == id_livro){
            *total = l.estoque;
            *disponiveis = l.estoque - contar_emprestimos_ativos(id_livro);
            if(*disponiveis < 0) *disponiveis = 0;
            fclose(livro);
            return 1;
        }
    }
    fclose(livro);
    return 0;
}

int salvar_estoque(int id_livro, int total){
    FILE *livro = fopen(ARQUIVO_LIVRO, "r");
    FILE *temp = fopen(ARQUIVO_TEMP, "w");
    char linha[300];
    Livro l;
    int encontrou = 0;
    if(livro == NULL || temp == NULL){
        if(livro != NULL) fclose(livro);
        if(temp != NULL) fclose(temp);
        printf("Erro ao abrir os arquivos de livros.\n");
        return 0;
    }
    if(fgets(linha, sizeof(linha), livro) != NULL) fputs("id;titulo;idautores;estoque\n", temp);
    while(fgets(linha, sizeof(linha), livro) != NULL){
        if(ler_livro_linha(linha, &l)){
            if(l.id_livro == id_livro){
                l.estoque = total;
                encontrou = 1;
            }
            fprintf(temp, "%d;%s;%d|%d|%d;%d\n", l.id_livro, l.nome_livro,
                    l.id_autores[0], l.id_autores[1], l.id_autores[2], l.estoque);
        } else fputs(linha, temp);
    }
    fclose(livro);
    fclose(temp);
    if(!encontrou){
        remove(ARQUIVO_TEMP);
        return 0;
    }
    remove(ARQUIVO_LIVRO);
    if(rename(ARQUIVO_TEMP, ARQUIVO_LIVRO) == 0) return 1;
    printf("Erro ao atualizar o estoque do livro.\n");
    return 0;
}

static int obter_status_usuario(int id_usuario, char *status){
    FILE *usuario = fopen(ARQUIVO_USUARIO, "r");
    char linha[300], nome[MAX_NOME], status_lido[20];
    int id;
    if(usuario == NULL) return 0;
    fgets(linha, sizeof(linha), usuario);
    while(fgets(linha, sizeof(linha), usuario) != NULL){
        if(sscanf(linha, "%d;%79[^;];%19[^\r\n]", &id, nome, status_lido) == 3 && id == id_usuario){
            strcpy(status, status_lido);
            fclose(usuario);
            return 1;
        }
    }
    fclose(usuario);
    return 0;
}

static int livro_cadastrado(int id_livro){
    FILE *livro = fopen(ARQUIVO_LIVRO, "r");
    char linha[300];
    Livro l;
    if(livro == NULL) return 0;
    fgets(linha, sizeof(linha), livro);
    while(fgets(linha, sizeof(linha), livro) != NULL){
        if(ler_livro_linha(linha, &l) && l.id_livro == id_livro){
            fclose(livro);
            return 1;
        }
    }
    fclose(livro);
    return 0;
}

void cadastrar_emprestimo(){
    FILE *emprestimo;
    char status[20];
    int id_usuario, id_livro, total, disponiveis, id_emprestimo;

    printf("Digite o ID do usuário:\n");
    scanf("%d", &id_usuario);
    if(!obter_status_usuario(id_usuario, status)){
        printf("Usuário não encontrado.\n");
        return;
    }
    if(strcmp(status, "SUSPENSO") == 0){
        printf("Usuário suspenso não pode realizar empréstimos.\n");
        return;
    }

    printf("Digite o ID do livro:\n");
    scanf("%d", &id_livro);
    if(!livro_cadastrado(id_livro)){
        printf("Livro não encontrado.\n");
        return;
    }
    buscar_estoque(id_livro, &total, &disponiveis);
    if(disponiveis <= 0){
        printf("Não há exemplares disponíveis deste livro.\n");
        return;
    }

    id_emprestimo = proximo_id(ARQUIVO_EMPRESTIMO);
    emprestimo = fopen(ARQUIVO_EMPRESTIMO, "a");
    if(emprestimo == NULL){
        printf("Erro ao abrir %s\n", ARQUIVO_EMPRESTIMO);
        return;
    }
    fprintf(emprestimo, "%d;%d;%d;PENDENTE\n", id_emprestimo, id_usuario, id_livro);
    fclose(emprestimo);
    printf("Empréstimo registrado.\n");
}

void alterar_emprestimo(int excluir){
    FILE *emprestimo = fopen(ARQUIVO_EMPRESTIMO, "r");
    FILE *temp = fopen(ARQUIVO_TEMP, "w");
    char linha[300];
    Emprestimo e;
    int id_alvo, encontrou = 0, pendente = 0;

    if(emprestimo == NULL || temp == NULL){
        if(emprestimo != NULL) fclose(emprestimo);
        if(temp != NULL) fclose(temp);
        printf("Erro ao abrir os arquivos de empréstimo.\n");
        return;
    }
    printf("Digite o ID do empréstimo que deseja %s:\n", excluir ? "excluir" : "devolver");
    scanf("%d", &id_alvo);
    if(fgets(linha, sizeof(linha), emprestimo) != NULL)
        fputs("id;id_usuario;id_livro;status\n", temp);
    while(fgets(linha, sizeof(linha), emprestimo) != NULL){
        if(!ler_emprestimo_linha(linha, &e)){
            fputs(linha, temp);
            continue;
        }
        if(e.id_emprestimo == id_alvo){
            if(excluir && strcmp(e.status, "PENDENTE") == 0) pendente = 1;
            else if(excluir) encontrou = 1;
            else if(strcmp(e.status, "PENDENTE") == 0){
                strcpy(e.status, "DEVOLVIDO");
                encontrou = 1;
            }
            if(excluir && strcmp(e.status, "PENDENTE") != 0) continue;
        }
        fprintf(temp, "%d;%d;%d;%s\n", e.id_emprestimo, e.id_usuario, e.id_livro, e.status);
    }
    fclose(emprestimo);
    fclose(temp);

    if(excluir && pendente){
        remove(ARQUIVO_TEMP);
        printf("Não é possível excluir um empréstimo pendente. Registre a devolução primeiro.\n");
    } else if(encontrou){
        remove(ARQUIVO_EMPRESTIMO);
        if(rename(ARQUIVO_TEMP, ARQUIVO_EMPRESTIMO) == 0)
            printf(excluir ? "Empréstimo excluído com sucesso.\n" : "Devolução registrada.\n");
        else printf("Erro ao atualizar o arquivo de empréstimos.\n");
    } else {
        remove(ARQUIVO_TEMP);
        printf(excluir ? "Empréstimo não encontrado.\n" : "Empréstimo pendente não encontrado.\n");
    }
}

void relatorio_emprestimos(){
    FILE *emprestimo = fopen(ARQUIVO_EMPRESTIMO, "r");
    char linha[300];
    Emprestimo e;

    if(emprestimo == NULL){
        printf("Erro ao abrir %s\n", ARQUIVO_EMPRESTIMO);
        return;
    }
    if(fgets(linha, sizeof(linha), emprestimo) != NULL){}
    printf("================ RELATÓRIO DE EMPRÉSTIMOS ================\n");
    while(fgets(linha, sizeof(linha), emprestimo) != NULL){
        if(ler_emprestimo_linha(linha, &e))
            printf("Empréstimo: %d | Usuário: %d | Livro: %d | Situação: %s\n",
                   e.id_emprestimo, e.id_usuario, e.id_livro, e.status);
    }
    fclose(emprestimo);
    pausar();
    limpar_tela();
}

void gerenciar_estoque(){
    FILE *livro = fopen(ARQUIVO_LIVRO, "r");
    char linha[300];
    Livro l;
    int total, disponiveis, id_alvo, quantidade;

    if(livro == NULL){
        printf("Erro ao abrir %s\n", ARQUIVO_LIVRO);
        return;
    }
    if(fgets(linha, sizeof(linha), livro) != NULL){}
    printf("ID     | LIVRO | TOTAL | DISPONÍVEIS\n");
    while(fgets(linha, sizeof(linha), livro) != NULL){
        if(ler_livro_linha(linha, &l)){
            buscar_estoque(l.id_livro, &total, &disponiveis);
            printf("%-6d | %s | %d | %d\n", l.id_livro, l.nome_livro, total, disponiveis);
        }
    }
    fclose(livro);

    printf("Digite o ID do livro para adicionar exemplares (0 para cancelar):\n");
    scanf("%d", &id_alvo);
    if(id_alvo == 0) return;
    if(!livro_cadastrado(id_alvo)){
        printf("Livro não encontrado.\n");
        return;
    }
    printf("Quantidade de exemplares a adicionar:\n");
    scanf("%d", &quantidade);
    if(quantidade <= 0){
        printf("Informe uma quantidade maior que zero.\n");
        return;
    }
    buscar_estoque(id_alvo, &total, &disponiveis);
    if(salvar_estoque(id_alvo, total + quantidade))
        printf("Estoque atualizado: %d exemplar(es) no total, %d disponível(is).\n",
               total + quantidade, disponiveis + quantidade);
}

int main(){
	garantir_arquivos();
    int opcao;
	while (1){
	 do{
         printf("+++++++++++++++++++++++++++++++++++++++++++++ \n"
	            "|SISBB - SISTEMA DE GESTÃO DE BIBLIOTECAS 🕮| \n"
	            "+++++++++++++++++++++++++++++++++++++++++++++ \n"
	            "1. Gestão de livros \n"
                "2. Gestão de autores \n"
	            "3. Gestão de usuários \n"
	            "4. Gestão de empréstimos \n"
	            "5. Relatórios \n"
	            "6. Sair \n"
	            "Escolha uma opção acima\n");
         scanf(" %d", &opcao);

		 if(opcao < 1 || opcao > 6){
			 printf("Digite uma opção válida");
             sleep(2);
		     limpar_tela();
		    }
	    }while(opcao < 1 || opcao > 6);
	 switch (opcao){
		 case 1:
		     limpar_tela();
		     printf("Escolha qual opção lhe atende\n"
			    "1 - Listar livros\n"
				"2 - Cadastrar livro\n"
				"3 - Atualizar livro\n"
				"4 - Excluir livro\n"
				"5 - Gerenciar estoque\n");
		     scanf(" %d", &opcao);
             limpar_buffer();
		     if(opcao == 1) listar_livro();
		     else if(opcao == 2) cadastrar_livro();
		     else if(opcao == 3) atualizar_livro();
			 else if(opcao == 4) excluir_livro();
			 else if(opcao == 5) gerenciar_estoque();
		     sleep(2);
			 limpar_tela();
		     break;		
		 case 2:
		     limpar_tela();
		     printf("Escolha qual opção lhe atende\n"
			    "1 - Listar autores\n"
				"2 - Cadastrar autor\n"
				"3 - Atualizar autor\n"
				"4 - Excluir autor\n");
		     scanf(" %d", &opcao);
		     if(opcao == 1) listar_autores();
		     else if(opcao == 2) cadastrar_autor();
		     else if(opcao == 3) atualizar_autor();
			 else if(opcao == 4) excluir_autor();
			 sleep(2);
		     limpar_tela();
		     break;
		 case 3:
		     limpar_tela(); 	
		     printf("Escolha qual opção lhe atende\n"
			    "1 - Listar usuários\n"
				"2 - Cadastrar usuário\n"
				"3 - Atualizar usuário\n"
				"4 - Excluir usuário\n");
		     scanf(" %d", &opcao);
		     if(opcao == 1) listar_usuarios();
		     else if(opcao == 2) cadastrar_usuario();
		     else if(opcao == 3) atualizar_usuario();
			 else if(opcao == 4) excluir_usuario();
		     sleep(2);
			 limpar_tela();
		     break;	
		 case 4:
		     limpar_tela();
		     printf("Escolha qual opção lhe atende\n"
			    "1 - Registrar empréstimo\n"
				"2 - Registrar devolução\n"
				"3 - Excluir empréstimo\n");
		     scanf(" %d", &opcao);
		     if(opcao == 1) cadastrar_emprestimo();
			 else if(opcao == 2) alterar_emprestimo(0);
			 else if(opcao == 3) alterar_emprestimo(1);
		     sleep(2);
		     limpar_tela();
			 break;
		 case 5:
             limpar_buffer();
		     relatorio_emprestimos();
			 break;
		 case 6:
		     printf("\nSaindo do programa...");
		     sleep(2);
		     return 0;
		  default:
		  	 break;	
	    }
     

	}



}
