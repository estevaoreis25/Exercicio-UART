#include <stdio.h>
#include <unistd.h>         //Used for UART
#include <fcntl.h>          //Used for UART
#include <termios.h> 
#include <string.h>   
#include "crc16.h"    //Used for UART

int menu();
void solicita_int();
void solicita_float();
void solicita_string();
void enviar_int();
void enviar_float();
void enviar_string();

int main() {
    int opcao;
    opcao = menu();
    while(opcao){
        switch(opcao){
        case 1:
            solicita_int();
            break;
        case 2:
            solicita_float();
            break;
        case 3:
            solicita_string();
            break;
        case 4:
            enviar_int();
            break;
        case 5:
            enviar_float();
            break;
        case 6:
            enviar_string();
            break;
        case 0:
            printf ("SAINDO\n");
            break;
        default :
            printf ("Valor invalido!\n");
        }
     opcao = menu();
    }
   return 0;
}

int menu(){
  printf("-------------Opções-----------\n");
  printf("1. Solicitar um INT\n");
  printf("2. Solicitar um FLOAT\n");
  printf("3. Solicitar uma STRING\n");
  printf("4. Enviar um INT\n");
  printf("5. Enviar um FLOAT\n");
  printf("6. Enviar uma STRING\n");
  printf("0. SAIR\n");
  printf("\n");
  printf("Digite o número da Opção desejada\n");
  int opcao;
  scanf("%d", &opcao);
  return opcao;
}

void solicita_int(){
    printf("Entrou na funcao 1\n");
  int uart0_filestream = -1;

    uart0_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);      //Open in non blocking read/write mode
    if (uart0_filestream == -1)
    {
        printf("Erro - Não foi possível iniciar a UART.\n");
    }
    else
    {
        printf("UART inicializada!\n");
    }    
    struct termios options;
    tcgetattr(uart0_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;     //<Set baud rate
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);

    unsigned char tx_buffer[20];
    unsigned char *p_tx_buffer;

    p_tx_buffer = &tx_buffer[0];
    *p_tx_buffer++ = 0x01;
    *p_tx_buffer++ = 0x23;
    *p_tx_buffer++ = 0xA1;
    *p_tx_buffer++ = 2;
    *p_tx_buffer++ = 6;
    *p_tx_buffer++ = 1;
    *p_tx_buffer++ = 6;
    short resposta = calcula_CRC(tx_buffer, 7);
    memcpy(p_tx_buffer, &resposta, sizeof(resposta));
    p_tx_buffer+=sizeof(resposta);

    for(int i =0; i < 9; i++ ){
        printf("%x DEGUG: %d\n", tx_buffer[i], i);
    }

    printf("Buffers de memória criados!\n");
    
    if (uart0_filestream != -1)
    {
        printf("Escrevendo caracteres na UART ...");
        int count = write(uart0_filestream, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));
        if (count < 0)
        {
            printf("UART TX error\n");
        }
        else
        {
            printf("escrito.\n");
        }
    }

    sleep(2);

    //----- CHECK FOR ANY RX BYTES -----
    if (uart0_filestream != -1)
    {
        // Read up to 255 characters from the port if they are there
        unsigned char rx_buffer[256];
        int rx_length = read(uart0_filestream, (void*)rx_buffer, sizeof(rx_buffer));      //Filestream, buffer to store in, number of bytes to read (max)
        if (rx_length < 0)
        {
            printf("Erro na leitura.\n"); //An error occured (will occur if there are no bytes)
        }
        else if (rx_length == 0)
        {
            printf("Nenhum dado disponível.\n"); //No data waiting
        }
        else
        {
            //Bytes received
            rx_buffer[rx_length] = '\0';
            for(int i =0; i < 10; i++ ){
                printf("%x DEGUG RX: %d\n", rx_buffer[i], i);
            }
            int result;
            memcpy(&result, &rx_buffer[3], sizeof(result));
            printf("%i Bytes lidos : %d\n", rx_length, result);
            short crcbuffer;
            memcpy(&crcbuffer, &rx_buffer[7], sizeof(short));
            short crcr = calcula_CRC(rx_buffer, 7);


            if(crcr == crcbuffer){
                printf("CRC Validado: %i", crcbuffer);
            }else{
                printf("CRC INVALIDO. \nCRC buffer: %i\nCRC CALC: %i\n", crcbuffer, crcr);
            }

        }
    }

    close(uart0_filestream);
}

void solicita_float(){
    int uart0_filestream = -1;

    uart0_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);      //Open in non blocking read/write mode
    if (uart0_filestream == -1)
    {
        printf("Erro - Não foi possível iniciar a UART.\n");
    }
    else
    {
        printf("UART inicializada!\n");
    }    
    struct termios options;
    tcgetattr(uart0_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;     //<Set baud rate
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);

    unsigned char tx_buffer[20];
    unsigned char *p_tx_buffer;
    
    p_tx_buffer = &tx_buffer[0];
    *p_tx_buffer++ = 0x01;
    *p_tx_buffer++ = 0x23;
    *p_tx_buffer++ = 0xA2;
    *p_tx_buffer++ = 2;
    *p_tx_buffer++ = 6;
    *p_tx_buffer++ = 1;
    *p_tx_buffer++ = 6;
    short resposta = calcula_CRC(tx_buffer, 7);
    memcpy(p_tx_buffer, &resposta, sizeof(resposta));
    p_tx_buffer+=sizeof(resposta);

    for(int i =0; i < 9; i++ ){
        printf("%x DEGUG: %d\n", tx_buffer[i], i);
    }


    printf("Buffers de memória criados!\n");
    
    if (uart0_filestream != -1)
    {
        printf("Escrevendo caracteres na UART ...");
        int count = write(uart0_filestream, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));
        if (count < 0)
        {
            printf("UART TX error\n");
        }
        else
        {
            printf("escrito.\n");
        }
    }

    sleep(1);

    //----- CHECK FOR ANY RX BYTES -----
    if (uart0_filestream != -1)
    {
         // Read up to 255 characters from the port if they are there
        unsigned char rx_buffer[256];
        int rx_length = read(uart0_filestream, (void*)rx_buffer, sizeof(rx_buffer));      //Filestream, buffer to store in, number of bytes to read (max)
        if (rx_length < 0)
        {
            printf("Erro na leitura.\n"); //An error occured (will occur if there are no bytes)
        }
        else if (rx_length == 0)
        {
            printf("Nenhum dado disponível.\n"); //No data waiting
        }
        else
        {
            //Bytes received
            rx_buffer[rx_length] = '\0';
            for(int i =0; i < 10; i++ ){
                printf("%x DEGUG RX: %d\n", rx_buffer[i], i);
            }
            float result;
            memcpy(&result, &rx_buffer[3], sizeof(result));
            printf("%i Bytes lidos : %f\n", rx_length, result);
            short crcbuffer;
            memcpy(&crcbuffer, &rx_buffer[7], sizeof(short));
            short crcr = calcula_CRC(rx_buffer, 7);


            if(crcr == crcbuffer){
                printf("CRC Validado: %i", crcbuffer);
            }else{
                printf("CRC INVALIDO. \nCRC buffer: %i\nCRC CALC: %i\n", crcbuffer, crcr);
            }

        }
    }

    close(uart0_filestream);
}

void solicita_string(){
    int uart0_filestream = -1;

    uart0_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);      //Open in non blocking read/write mode
    if (uart0_filestream == -1)
    {
        printf("Erro - Não foi possível iniciar a UART.\n");
    }
    else
    {
        printf("UART inicializada!\n");
    }    
    struct termios options;
    tcgetattr(uart0_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;     //<Set baud rate
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);

    unsigned char tx_buffer[20];
    unsigned char *p_tx_buffer;
    
    p_tx_buffer = &tx_buffer[0];
    *p_tx_buffer++ = 0x01;
    *p_tx_buffer++ = 0x23;
    *p_tx_buffer++ = 0xA3;
    *p_tx_buffer++ = 2;
    *p_tx_buffer++ = 6;
    *p_tx_buffer++ = 1;
    *p_tx_buffer++ = 6;
    short resposta = calcula_CRC(tx_buffer, 7);
    memcpy(p_tx_buffer, &resposta, sizeof(resposta));
    p_tx_buffer+=sizeof(resposta);

    printf("Buffers de memória criados!\n");
    
    if (uart0_filestream != -1)
    {
        printf("Escrevendo caracteres na UART ...");
        int count = write(uart0_filestream, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));
        if (count < 0)
        {
            printf("UART TX error\n");
        }
        else
        {
            printf("escrito.\n");
        }
    }

    sleep(1);

    //----- CHECK FOR ANY RX BYTES -----
    if (uart0_filestream != -1)
    {
        // Read up to 255 characters from the port if they are there
        unsigned char rx_buffer[256];
        int rx_length = read(uart0_filestream, (void*)rx_buffer, sizeof(rx_buffer));      //Filestream, buffer to store in, number of bytes to read (max)
        if (rx_length < 0)
        {
            printf("Erro na leitura.\n"); //An error occured (will occur if there are no bytes)
        }
        else if (rx_length == 0)
        {
            printf("Nenhum dado disponível.\n"); //No data waiting
        }
        else
        {
            //Bytes received
            rx_buffer[rx_length] = '\0';
            for(int i =0; i < rx_length; i++ ){
                printf("%x DEGUG RX: %d\n", rx_buffer[i], i);
            }
            char result[255];
            memcpy(&result, &rx_buffer[4], rx_buffer[3]);
            printf("%i Bytes lidos : %s\n", rx_length, result);

            short crcbuffer;
            memcpy(&crcbuffer, &rx_buffer[5+rx_buffer[3]], sizeof(short));
            short crcr = calcula_CRC(rx_buffer, 5+rx_buffer[3]);


            if(crcr == crcbuffer){
                printf("CRC Validado: %i", crcbuffer);
            }else{
                printf("CRC INVALIDO. \nCRC buffer: %i\nCRC CALC: %i\n", crcbuffer, crcr);
            }
        }
    }

    close(uart0_filestream);
}


void enviar_int(){
  int uart0_filestream = -1;

    uart0_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);      //Open in non blocking read/write mode
    if (uart0_filestream == -1)
    {
        printf("Erro - Não foi possível iniciar a UART.\n");
    }
    else
    {
        printf("UART inicializada!\n");
    }    
    struct termios options;
    tcgetattr(uart0_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;     //<Set baud rate
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);

    unsigned char tx_buffer[20];
    unsigned char *p_tx_buffer;
    int num = 1618;
    p_tx_buffer = &tx_buffer[0];
    *p_tx_buffer++ = 0x01;
    *p_tx_buffer++ = 0x16;
    *p_tx_buffer++ = 0xB1;
    memcpy(p_tx_buffer, &num, sizeof(num));
    p_tx_buffer+=sizeof(num);
    short resposta = calcula_CRC(tx_buffer, 7);
    memcpy(p_tx_buffer, &resposta, sizeof(resposta));
    p_tx_buffer+=sizeof(resposta);

    printf("Buffers de memória criados!\n");
    
    if (uart0_filestream != -1)
    {
        printf("Escrevendo caracteres na UART ...");
        int count = write(uart0_filestream, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));
        if (count < 0)
        {
            printf("UART TX error\n");
        }
        else
        {
            printf("escrito.\n");
        }
    }

    sleep(1);

    //----- CHECK FOR ANY RX BYTES -----
    if (uart0_filestream != -1)
    {
                // Read up to 255 characters from the port if they are there
        unsigned char rx_buffer[256];
        int rx_length = read(uart0_filestream, (void*)rx_buffer, sizeof(rx_buffer));      //Filestream, buffer to store in, number of bytes to read (max)
        if (rx_length < 0)
        {
            printf("Erro na leitura.\n"); //An error occured (will occur if there are no bytes)
        }
        else if (rx_length == 0)
        {
            printf("Nenhum dado disponível.\n"); //No data waiting
        }
        else
        {
            //Bytes received
            rx_buffer[rx_length] = '\0';
            for(int i =0; i < 10; i++ ){
                printf("%x DEGUG RX: %d\n", rx_buffer[i], i);
            }
            int result;
            memcpy(&result, &rx_buffer[3], sizeof(result));
            printf("%i Bytes lidos : %d\n", rx_length, result);
            short crcbuffer;
            memcpy(&crcbuffer, &rx_buffer[7], sizeof(short));
            short crcr = calcula_CRC(rx_buffer, 7);


            if(crcr == crcbuffer){
                printf("CRC Validado: %i", crcbuffer);
            }else{
                printf("CRC INVALIDO. \nCRC buffer: %i\nCRC CALC: %i\n", crcbuffer, crcr);
            }

        }
    }

    close(uart0_filestream);
}

void enviar_float(){
  int uart0_filestream = -1;

    uart0_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);      //Open in non blocking read/write mode
    if (uart0_filestream == -1)
    {
        printf("Erro - Não foi possível iniciar a UART.\n");
    }
    else
    {
        printf("UART inicializada!\n");
    }    
    struct termios options;
    tcgetattr(uart0_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;     //<Set baud rate
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);

    unsigned char tx_buffer[20];
    unsigned char *p_tx_buffer;
    float num = 1618.5f;
    p_tx_buffer = &tx_buffer[0];
    *p_tx_buffer++ = 0x01;
    *p_tx_buffer++ = 0x16;
    *p_tx_buffer++ = 0xB2;
    memcpy(p_tx_buffer, &num, sizeof(num));
    p_tx_buffer+=sizeof(num);
    short resposta = calcula_CRC(tx_buffer, 7);
    memcpy(p_tx_buffer, &resposta, sizeof(resposta));
    p_tx_buffer+=sizeof(resposta);

    printf("Buffers de memória criados!\n");
    
    if (uart0_filestream != -1)
    {
        printf("Escrevendo caracteres na UART ...");
        int count = write(uart0_filestream, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));
        if (count < 0)
        {
            printf("UART TX error\n");
        }
        else
        {
            printf("escrito.\n");
        }
    }

    sleep(1);

    //----- CHECK FOR ANY RX BYTES -----
    if (uart0_filestream != -1)
    {
         // Read up to 255 characters from the port if they are there
        unsigned char rx_buffer[256];
        int rx_length = read(uart0_filestream, (void*)rx_buffer, sizeof(rx_buffer));      //Filestream, buffer to store in, number of bytes to read (max)
        if (rx_length < 0)
        {
            printf("Erro na leitura.\n"); //An error occured (will occur if there are no bytes)
        }
        else if (rx_length == 0)
        {
            printf("Nenhum dado disponível.\n"); //No data waiting
        }
        else
        {
            //Bytes received
            rx_buffer[rx_length] = '\0';
            for(int i =0; i < 10; i++ ){
                printf("%x DEGUG RX: %d\n", rx_buffer[i], i);
            }
            float result;
            memcpy(&result, &rx_buffer[3], sizeof(result));
            printf("%i Bytes lidos : %f\n", rx_length, result);
            short crcbuffer;
            memcpy(&crcbuffer, &rx_buffer[7], sizeof(short));
            short crcr = calcula_CRC(rx_buffer, 7);


            if(crcr == crcbuffer){
                printf("CRC Validado: %i", crcbuffer);
            }else{
                printf("CRC INVALIDO. \nCRC buffer: %i\nCRC CALC: %i\n", crcbuffer, crcr);
            }

        }
    }

    close(uart0_filestream);
}

void enviar_string(){
  int uart0_filestream = -1;

    uart0_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);      //Open in non blocking read/write mode
    if (uart0_filestream == -1)
    {
        printf("Erro - Não foi possível iniciar a UART.\n");
    }
    else
    {
        printf("UART inicializada!\n");
    }    
    struct termios options;
    tcgetattr(uart0_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;     //<Set baud rate
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);

    unsigned char tx_buffer[20];
    unsigned char *p_tx_buffer;
    unsigned char text[] = "OLA UART";
    p_tx_buffer = &tx_buffer[0];
    *p_tx_buffer++ = 0xB3;
    *p_tx_buffer++ = sizeof(text);
    memcpy(p_tx_buffer, text, sizeof(text));
    p_tx_buffer+=sizeof(text);
    *p_tx_buffer++ = 2;
    *p_tx_buffer++ = 6;
    *p_tx_buffer++ = 1;
    *p_tx_buffer++ = 6;

    printf("Buffers de memória criados!\n");
    
    if (uart0_filestream != -1)
    {
        printf("Escrevendo caracteres na UART ...");
        int count = write(uart0_filestream, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));
        if (count < 0)
        {
            printf("UART TX error\n");
        }
        else
        {
            printf("escrito.\n");
        }
    }

    sleep(1);

    //----- CHECK FOR ANY RX BYTES -----
    if (uart0_filestream != -1)
    {
        // Read up to 255 characters from the port if they are there
        float rx_buffer;
        int rx_length = read(uart0_filestream, (void*)&rx_buffer, sizeof(rx_buffer));      //Filestream, buffer to store in, number of bytes to read (max)
        if (rx_length < 0)
        {
            printf("Erro na leitura.\n"); //An error occured (will occur if there are no bytes)
        }
        else if (rx_length == 0)
        {
            printf("Nenhum dado disponível.\n"); //No data waiting
        }
        else
        {
            //Bytes received
            //rx_buffer[rx_length] = '\0';
            printf("%i Bytes lidos : %f\n", rx_length, rx_buffer);
        }
    }

    close(uart0_filestream);
}