#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "client.h" 
#include <ctype.h>
#include <time.h>

#define MSG_MAX_SIZE 350
#define BUFFER_SIZE (MSG_MAX_SIZE + 100)
#define LOGIN_MAX_SIZE 13
#define HIST_MAX_SIZE 200
#define LARGURA_TELA 640
#define ALTURA_TELA 480


ALLEGRO_BITMAP *fundo = NULL;
ALLEGRO_BITMAP *imagem1 = NULL;
ALLEGRO_BITMAP *imagem2 = NULL;
ALLEGRO_BITMAP *peixe1 = NULL;
ALLEGRO_BITMAP *peixe2 = NULL;
ALLEGRO_BITMAP *peixe3 = NULL;
ALLEGRO_BITMAP *peixe4 = NULL;
ALLEGRO_BITMAP *racao = NULL;
ALLEGRO_BITMAP *isca = NULL;
ALLEGRO_BITMAP *isca_a = NULL;
ALLEGRO_BITMAP *personagem_1 = NULL;
ALLEGRO_BITMAP *personagem_2 = NULL;
ALLEGRO_BITMAP *personagem_3 = NULL;
ALLEGRO_BITMAP *personagem_4 = NULL;
ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_FONT *fonte = NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;


typedef struct {
  int X,Y,permissao,id,tecla,pers,tamanho,xb,yb,fome,direcao,x_aux,y_aux;
  char nome[30];
}data;


char str[30] = {"192.168.15.166"};
bool sair = false;
bool concluido = false;
int bx=1,by=0;
int fome=0,tamanho=0;
int ant_dir [6] ={0,0,0,0,0,0};
int matriz_tamanho [ALTURA_TELA] [LARGURA_TELA];
// matriz marcao é a matriz onde é salva a posição de todos os jogadores.
// map é o mapa que é copiado para a matriz tela para mostrar no terminal.
char marcao [ALTURA_TELA] [LARGURA_TELA] ,map [ALTURA_TELA] [LARGURA_TELA];
data dados;
// posição inicial dos outros playes, pode ser qualquer valor pois eles vão ser enviados para matriz marcão e serão
// apagados como xAnterior e yAnterior.
int id,Peixe_eu=1;
int xA[13]={1,1,1,1,1,1,1,1,1,1,1,1,1},yA[13]={1,1,1,1,1,1,1,1,1,1,1,1,1};
int x,y,indicador_X_Bit=0,indicador_Y_Bit=0;
int x_bit[2][5] = {{0,160,320,480,640},{1440,1280,1120,960,800}},y_bit[5] = {0,129,260,391,522};
int x_bit_isca[10]={0,77,154,231,308,385,462,539,616,693},xi=0;
char res[2]={32,'\0'};
bool pedir_nome = false,right=true,left=false;
// os personagens são colocados de arcordo com o seu id.
char pers[2] [2] [5] = { { {'L','U','I','S'},{'P','E','R','A'} } , { {'l','u','i','s'},{'p','e','r','a'} } };
// monta as matrizes de mapa que é copiada para tela. A marcação recebe espaços vazios.
void monta ();
void GeraPosicao();
void mostraTela();
void marcaPosicao(int xx, int yy, int px,int py, int personagem,int dir,int camada,int tamanho_peixe);
enum conn_ret_t tryConnect();
bool assertConnection();
void runGame();
int maximo();
void telaInicial();
char manipular_char(ALLEGRO_EVENT evento);
int conectar();
bool inicializar();
bool carregar_arquivos();
void finalizar();
void manipular_entrada();
void exibir_texto_centralizado();


int main() {
    inicializar();
    carregar_arquivos();

    GeraPosicao();
    monta();
    
    dados.X=x;
    dados.Y=y;
    telaInicial(); 
      
    //while(!sair) {
        // roda game.
        runGame();
        
        //printf("Server disconnected! Want to try again? [Y/N] ");
        int ress;
        
    //}
    finalizar();
    return 0;
}

void monta () {
    int i=0,j=0;

    for( i = 0 ; i < ALTURA_TELA  ; i++ ){

        for( j = 0 ; j < LARGURA_TELA ; j++ ){
            map[i] [j] =32;
            marcao [i] [j] =32;
            matriz_tamanho [i] [j] = 0;
        }
    }
    map[40] [530] ='i';
    map[420] [240] ='i';
    map[310] [120] ='i';
}

// recebe a posição anterior para apagar e, a próxima "px" e "py" para marcar o personagem.  
void marcaPosicao(int xx, int yy, int px,int py, int personagem, int dir,int camada,int tamanho_peixe){
    printf("%c\n",pers[camada][dir][personagem] );
    marcao[py] [px] = pers[camada][dir][personagem];
    marcao[yy] [xx] = 32;
    matriz_tamanho [py] [px] = tamanho_peixe;
    matriz_tamanho [yy] [xx] = 0;
}
// Gera posicao aleatória para começo do jogo.
void GeraPosicao(){

    do{
        
            srand((unsigned)time(NULL));
            x=rand()%630;  
            y=rand()%470;
        
    } while ( x < 10 || y < 10);
    
}
// faz uma cópia da matriz map na matriz tela e depois verifica em que posições na matriz macação estão
// os jogadores. 
void mostraTela(int primeiro_X){
    int i,j;
    al_draw_bitmap(fundo,0,0,0);
    for(i=0;i<ALTURA_TELA;i++){
        for(j=0;j<LARGURA_TELA;j++){

            if(marcao[i] [j] == 'L'){
                personagem_1 = al_create_sub_bitmap(peixe1, x_bit [0][primeiro_X], y_bit[matriz_tamanho[i][j]], 160,131);
                al_draw_bitmap(personagem_1, j-79,i-65,0);
            }
            else if(marcao[i] [j] == 'l'){
                personagem_1 = al_create_sub_bitmap(peixe1, x_bit [1][5], y_bit[matriz_tamanho[i][j]], 160,131);
                al_draw_bitmap(personagem_1, j-79,i-65,0);
            }
            else if(marcao[i] [j] == 'P'){
                personagem_1 = al_create_sub_bitmap(peixe1, x_bit [1][primeiro_X], y_bit[matriz_tamanho[i][j]], 160,131);
                al_draw_bitmap(personagem_1, j-79,i-65,0);
            }
            else if(marcao[i] [j] == 'p'){
                personagem_1 = al_create_sub_bitmap(peixe1, x_bit [0][5], y_bit[matriz_tamanho[i][j]], 160,131);
                al_draw_bitmap(personagem_1, j-79,i-65,0);
            }
            else if(marcao[i] [j] == 'U'){
                personagem_2 = al_create_sub_bitmap(peixe2, x_bit [0][primeiro_X], y_bit[matriz_tamanho[i][j]], 160,131);
                al_draw_bitmap(personagem_2, j-79,i-65,0);
            }
            else if(marcao[i] [j] == 'u'){
                personagem_2 = al_create_sub_bitmap(peixe2, x_bit [1][5], y_bit[matriz_tamanho[i][j]], 160,131);
                al_draw_bitmap(personagem_2, j-79,i-65,0);
            }
            else if(marcao[i] [j] == 'E'){
                personagem_2 = al_create_sub_bitmap(peixe2, x_bit [1][primeiro_X], y_bit[matriz_tamanho[i][j]], 160,131);
                al_draw_bitmap(personagem_2, j-79,i-65,0);
            }
            else if(marcao[i] [j] == 'e'){
                personagem_2 = al_create_sub_bitmap(peixe2, x_bit [0][5], y_bit[matriz_tamanho[i][j]], 160,131);
                al_draw_bitmap(personagem_2, j-79,i-65,0);
            }
            else if(marcao[i] [j] == 'I'){
                personagem_3 = al_create_sub_bitmap(peixe3, x_bit [0][primeiro_X], y_bit[matriz_tamanho[i][j]], 160,131);
                al_draw_bitmap(personagem_3,j-79,i-65,0);
            }
            else if(marcao[i] [j] == 'R'){
                personagem_3 = al_create_sub_bitmap(peixe3, x_bit [1][primeiro_X], y_bit[matriz_tamanho[i][j]], 160,131);
                al_draw_bitmap(personagem_3, j-79,i-65,0);
            }
            else if(marcao[i] [j] == 'S'){
                personagem_4 = al_create_sub_bitmap(peixe4, x_bit [0][primeiro_X], y_bit[matriz_tamanho[i][j]], 160,131);
                al_draw_bitmap(personagem_4, j-79,i-65,0);
            }
            else if(marcao[i] [j] == 'A'){
                personagem_4 = al_create_sub_bitmap(peixe4, x_bit [1][primeiro_X], y_bit[matriz_tamanho[i][j]], 160,131);
                al_draw_bitmap(personagem_4, j-79,i-65,0);
            }

            if(map[i] [j] == 'i'){
                isca_a = al_create_sub_bitmap(isca, x_bit_isca [xi],0, 77,76);
                al_draw_bitmap(isca_a, j-38,i-37,0);
            }
            else if (map[i] [j] == 'r'){
                al_draw_bitmap(racao, j-2,i-2,0);
            }
        }
    }
    al_flip_display();
}


// tenta se conectar ao servidor.

void runGame() {
    char ch,i,j;
    ch='l';
    printf("%d %d\n",x,y );
    marcaPosicao(0,0,x,y,Peixe_eu,0,0,0);
    mostraTela(0);
    while (1) {
        // se alguma tecla for pressionada e for diferente de 'l', envia posição, id e tecla.
        while (!al_is_event_queue_empty(fila_eventos)){
            
            ALLEGRO_EVENT evento;
            al_get_next_event(fila_eventos, &evento);
            if (evento.type == ALLEGRO_EVENT_KEY_DOWN){
                    dados.tecla = evento.keyboard.keycode;
                    //printf("%d\n",dados.tecla );
                    dados.id=id;
                    dados.X=x;
                    dados.Y=y;
                    dados.fome=fome;
                    dados.tamanho=tamanho;
                    dados.pers=Peixe_eu;
                    dados.direcao=ant_dir[id];
                    sendMsgToServer((void *)&dados,sizeof(data));
            }
            else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
            sair = true; 
            return;
            }
            //mostraTela(x_bit[bx],y_bit[by]);
        }
        
        // receber mensagem do servidor
        int ret = recvMsgFromServer(&dados,DONT_WAIT);
        // se receber server diconect, acaba com a função
        if (ret == SERVER_DISCONNECTED) {
          return;
        } 
        // se houver mensagem, ler os dados:
        else if (ret != NO_MESSAGE) {
            //printf("per %d id %d \n", dados.permissao,dados.id);
             // se a permição para o pedido enviado for igual a 1, o client executa o que pediu para fazer.
             if(dados.permissao == 1 && id == dados.id){
                
                if(dados.tecla==83){//left
                    marcaPosicao(x,y,dados.X,dados.Y,Peixe_eu,0,0, dados.tamanho);
                    ant_dir[dados.id]=0;
                }
                else if(dados.tecla==82){//right
                    marcaPosicao(x,y,dados.X,dados.Y,Peixe_eu,1,0,dados.tamanho);
                    ant_dir[dados.id]=1;
                }
                else if(dados.tecla==23){

                    marcaPosicao(x,y,dados.X,dados.Y, Peixe_eu,dados.direcao,1,dados.tamanho);
                    map[dados.y_aux] [dados.x_aux]=32;
                    fome=dados.fome;
                    tamanho = dados.tamanho;
                    printf("fome %d\n", fome );
                }
                else {
                    marcaPosicao(x,y,dados.X,dados.Y,Peixe_eu,ant_dir[dados.id],0,dados.tamanho);

                }
                 x=dados.X;
                 y=dados.Y;
                 //printf("x %d\n",x );
                 
            }
            // se receber mensagem do server, mas o id não for o seu, mostra essa posição como sendo outro jogador.
            if(dados.permissao == 1 && id != dados.id && dados.id != 6){
                if(dados.tecla==83){
                    marcaPosicao(xA[dados.id],yA[dados.id],dados.X,dados.Y,dados.pers,0,0,dados.tamanho);
                    ant_dir[dados.id]=0;
                }
                else if(dados.tecla==82){
                    marcaPosicao(xA[dados.id],yA[dados.id],dados.X,dados.Y,dados.pers,1,0,dados.tamanho);
                    ant_dir[dados.id]=1;
                }
                else if(dados.tecla==23){

                    marcaPosicao(xA[dados.id],yA[dados.id],dados.X,dados.Y,dados.pers,dados.direcao,1,dados.tamanho);
                    map[dados.y_aux] [dados.x_aux]=32;
                }
                else {
                    marcaPosicao(xA[dados.id],yA[dados.id],dados.X,dados.Y,dados.pers,ant_dir[dados.id],0,dados.tamanho);
                }

                xA[dados.id]=dados.X;
                yA[dados.id]=dados.Y;
            }
            if(dados.id==6){
                xi++;
                bx=dados.xb;
                map[dados.Y] [dados.X] = 'r';
                if(xi>=6)
                    xi=0;
            }
        }
        mostraTela(bx);
    }
}
int maximo(int h){
    if(h<=0){
        return 0;
    }
    else if(h>=255){
        return 255;
    }
    return h;

}
//enum conn_ret_t tryConnect() {
  //printf("Please enter the server IP: ");
//  return connectToServer(str);
//}
/*
int conecta(){
  

  if(ans==SERVER_UP)
    return 1;
  else if(ans == SERVER_DOWN)
    return 2;
  else if(ans==SERVER_FULL)
    return 3;
  else if(ans == SERVER_CLOSED)
    return 4;
  else 
    return 0;s
}
*/
void telaInicial(){
   
    int i;
    // enquanto o server não se conectar
    while (!concluido) {
        
        while (!al_is_event_queue_empty(fila_eventos)){
            

            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);
 
            if (!concluido){
                manipular_entrada(evento);
                 
                al_flip_display();
                if (evento.type == ALLEGRO_EVENT_KEY_DOWN && evento.keyboard.keycode == ALLEGRO_KEY_ENTER){
                    
                    enum conn_ret_t ans = connectToServer(str);

                    if(ans == SERVER_UP){
                            strcpy(str,"");

                            while(!pedir_nome){
                                while (!al_is_event_queue_empty(fila_eventos)){
                                
                                            ALLEGRO_EVENT evento;
                                            al_wait_for_event(fila_eventos, &evento);
                                 
                                            if (!pedir_nome){
                                                manipular_entrada(evento);
                                                
                                                if (evento.type == ALLEGRO_EVENT_KEY_DOWN && evento.keyboard.keycode == ALLEGRO_KEY_ENTER){
                                                    strcpy(dados.nome,str);
                                                    sendMsgToServer((void *)&dados,sizeof(data));
                                                    // espera mensagem do servidor. O servidor é quem vai definir o id desse client, ele precisa desse ip para se identificar.
                                                    recvMsgFromServer(&dados,WAIT_FOR_IT);
                                                    // salva id recebido.
                                                    id=dados.id;
                                                    pedir_nome = true;
                                                    dados.tecla=83;
                                                    dados.id=id;
                                                    dados.X=x;
                                                    dados.Y=y;
                                                    dados.fome=fome;
                                                    dados.tamanho=tamanho;
                                                    dados.pers=Peixe_eu;
                                                    dados.direcao=ant_dir[id];
                                                    sendMsgToServer((void *)&dados,sizeof(data));
                                                }
                                                

                                            }
                                                                                          
                                            if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                                                concluido = true;
                                                pedir_nome - true;
                                                sair = true; 
                                            }
                                }

                                if(!pedir_nome){
                                    al_draw_bitmap(imagem1, 0, 0, 0);
                                    al_draw_text(fonte, al_map_rgb(0,0,0), 180- al_get_font_ascent(fonte),
                                    (580 - al_get_font_ascent(fonte)) / 2,
                                    ALLEGRO_ALIGN_CENTRE + al_get_font_ascent(fonte), str);
                                    al_draw_bitmap(imagem2, 0, 0, 0);
                                    exibir_texto_centralizado("Nome",290,450);
                                    al_flip_display();
                                }
                            }
                        
                      concluido=true;
                      //pede nome.
                    }
                    else if (ans == SERVER_DOWN) {
                        al_draw_bitmap(imagem1, 0, 0, 0);
                        al_draw_bitmap(imagem2, 0, 0, 0);
                        exibir_texto_centralizado("Server is down!",180 ,450);
                        al_flip_display();
                        strcpy(str,"");
                        for(i=0;i<100000000;i++);    
                    } 
                    else if (ans==SERVER_FULL) {
                      al_draw_bitmap(imagem1, 0, 0, 0);
                      al_draw_bitmap(imagem2, 0, 0, 0);
                      exibir_texto_centralizado("Server is full!",290,450);
                      al_flip_display();
                      strcpy(str,"");
                      for(i=0;i<100000000;i++);  
                    } 
                    else if (ans==SERVER_CLOSED) {
                      al_draw_bitmap(imagem1, 0, 0, 0);
                      al_draw_bitmap(imagem2, 0, 0, 0);
                      exibir_texto_centralizado("Server is closed for new connections!", 290,450);
                      al_flip_display();
                      strcpy(str,"");
                      for(i=0;i<100000000;i++);  
                    } 
                    else {
                      al_draw_bitmap(imagem1, 0, 0, 0);
                      al_draw_bitmap(imagem2, 0, 0, 0);
                      exibir_texto_centralizado("Server didn't respond to connection!",290,450);
                      al_flip_display();
                      strcpy(str,"");
                      for(i=0;i<100000000;i++); 
                    }
                    if(ans!=SERVER_UP){
                      strcpy(str,"");
                    }
                }
            }
            
            if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                concluido = true;
                sair = true;
                return; 
            }
        }
        if(!concluido){
          al_draw_bitmap(imagem1, 0, 0, 0);
          al_draw_text(fonte, al_map_rgb(0,0,0), 180- al_get_font_ascent(fonte),
          (580 - al_get_font_ascent(fonte)) / 2,
          ALLEGRO_ALIGN_CENTRE + al_get_font_ascent(fonte), str);
          al_draw_bitmap(imagem2, 0, 0, 0);
          exibir_texto_centralizado("IP",290,450);
          al_flip_display();
        }
    }
}
 
bool inicializar(){

    if (!al_init()){
        fprintf(stderr, "Falha ao inicializar Allegro.\n");
        return false;
    }
 
    al_init_font_addon();
 
    if (!al_init_ttf_addon()){
        fprintf(stderr, "Falha ao inicializar add-on allegro_ttf.\n");
        return false;
    }
    al_init_primitives_addon();
    
    if (!al_init_primitives_addon()){
        fprintf(stderr, "Falha ao inicializar primitives\n");
        return false;
    }
 
    if (!al_init_image_addon()){
        fprintf(stderr, "Falha ao inicializar add-on allegro_image.\n");
        return false;
    }

    if (!al_install_keyboard()){
        fprintf(stderr, "Falha ao inicializar o teclado.\n");
        return false;
    }

    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    
    if (!janela){
        fprintf(stderr, "Falha ao criar janela.\n");
        return false;
    }
    // Torna apto o uso de mouse na aplicação
    if (!al_install_mouse())
    {
        fprintf(stderr, "Falha ao inicializar o mouse.\n");
        al_destroy_display(janela);
        return -1;
    }
 
    // Atribui o cursor padrão do sistema para ser usado
    if (!al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT))
    {
        fprintf(stderr, "Falha ao atribuir ponteiro do mouse.\n");
        al_destroy_display(janela);
        return -1;
    }

    al_set_window_title(janela, "Aquar.io");
 
    fila_eventos = al_create_event_queue();
    if (!fila_eventos){ 
        fprintf(stderr, "Falha ao criar fila de eventos.\n");
        al_destroy_display(janela);
        return false;
    }
 
    // Dizemos que vamos tratar os eventos vindos do mouse
    //al_register_event_source(fila_eventos, al_get_mouse_event_source());  
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_display_event_source(janela));
 
    return true;
}

bool carregar_arquivos(){
    fundo = al_load_bitmap("examples/AllegroAquar/Resources/Etc/fundo.png");
    
    if (!fundo){
        fprintf(stderr, "Falha ao carregar \"fundo.jpg\".\n");
        return false;
    }
    imagem1 = al_load_bitmap("examples/AllegroAquar/Resources/Etc/tuba.png");
    
    if (!imagem1){
        fprintf(stderr, "Falha ao carregar \"tuba.jpg\".\n");
        return false;
    }
    imagem2 = al_load_bitmap("examples/AllegroAquar/Resources/Etc/dentes.png");
    
    if (!imagem2){
        fprintf(stderr, "Falha ao carregar \"dente.png\".\n");
        return false;
    }
    peixe1 = al_load_bitmap ("examples/AllegroAquar/Resources/Tilesets/peixe1.png");
    if(!peixe1){
        fprintf(stderr, "Falha ao carregar \"peixe1.png\". \n");
        return false;
    }
    peixe2 = al_load_bitmap ("examples/AllegroAquar/Resources/Tilesets/peixe2.png");
    if(!peixe1){
        fprintf(stderr, "Falha ao carregar \"peixe2.png\". \n");
        return false;
    }
    /*
    peixe3 = al_load_bitmap ("examples/AllegroAquar/Resources/Tilesets/peixe3.png");
    if(!peixe3){
        fprintf(stderr, "Falha ao carregar \"peixe3.png\". \n");
        return false;
    }
    peixe4 = al_load_bitmap ("examples/AllegroAquar/Resources/Tilesets/peixe4.png");
    if(!peixe1){
        fprintf(stderr, "Falha ao carregar \"peixe4.png\". \n");
        return false;
    }
    */
    isca = al_load_bitmap ("examples/AllegroAquar/Resources/Tilesets/isca.png");
    if(!isca){
        fprintf(stderr, "Falha ao carregar \"isca.png\". \n");
        return false;
    }
    racao = al_load_bitmap ("examples/AllegroAquar/Resources/Tilesets/racao.png");
    if(!racao){
        fprintf(stderr, "Falha ao carregar \"racao.png\". \n");
        return false;
    }
    fonte = al_load_font("examples/AllegroAquar/Resources/Fonts/Ubuntu-R.ttf", 42, 0);
    
    if (!fonte){
        fprintf(stderr, "Falha ao carregar \"Ubuntu-R.ttf\".\n");
        return false;
    }
 
    return true;
}
 
void finalizar()
{
    al_destroy_bitmap(fundo);
    al_destroy_bitmap(imagem1);
    al_destroy_bitmap(imagem2);
    al_destroy_font(fonte);
    al_destroy_event_queue(fila_eventos);
    al_destroy_display(janela);
}
 
void manipular_entrada(ALLEGRO_EVENT evento){

    if (evento.type == ALLEGRO_EVENT_KEY_CHAR){
        
        if (strlen(str) <= 29){
            char temp[] = {evento.keyboard.unichar, '\0'};
            if (evento.keyboard.unichar == ' '){
                strcat(str, temp);
            }
            else if (evento.keyboard.unichar >= '0' &&
                     evento.keyboard.unichar <= '9'){
                strcat(str, temp);
            }
            else if (evento.keyboard.unichar == '.'){
                strcat(str, temp);
            }
            else if (evento.keyboard.unichar >= 'A' &&
                     evento.keyboard.unichar <= 'Z'){
                strcat(str, temp);
            }
            else if (evento.keyboard.unichar >= 'a' &&
                     evento.keyboard.unichar <= 'z'){
                strcat(str, temp);
            }
        }
        if (evento.keyboard.keycode == ALLEGRO_KEY_BACKSPACE && strlen(str) != 0){
            str[strlen(str) - 1] = '\0';
        }
    }
}
char manipular_char(ALLEGRO_EVENT evento){

    if (evento.type == ALLEGRO_EVENT_KEY_CHAR){
        
            if (evento.keyboard.unichar >= 'A' &&
                     evento.keyboard.unichar <= 'Z'){
                res [0]= evento.keyboard.unichar;
            }
            else if (evento.keyboard.unichar >= 'a' &&
                     evento.keyboard.unichar <= 'z'){
                res[0]= evento.keyboard.unichar;
            }
            else 
              res[0]= '0';
        }
}
 
void exibir_texto_centralizado(char texto [30], int linha, int coluna ){
    
    if (strlen(texto) > 0){
        al_draw_text(fonte, al_map_rgb(255,255,255), linha,
        (coluna - al_get_font_ascent(fonte)) / 2,
        ALLEGRO_ALIGN_CENTRE + al_get_font_ascent(fonte), texto);
    }
}
