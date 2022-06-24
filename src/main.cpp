#include <PS4Controller.h>
#include <Servo.h>

#define motor_1 13
#define motor_2 4
#define m1_horario 12
#define m1_antihorario 14
#define m2_horario 15
#define m2_antihorario 2
#define led_1 18
#define led_2 19
#define buzzer 26
#define servoPin 32

Servo servo1;

int freq = 3000;
int freq_buzzer = 4000;
int canal_R2 = 0;
int canal_L2 = 1;
int canal_buzzer = 2;
int velocidade_normal = 3250;
int velocidade_sport = 4095;
int resolution = 12;
int resolution_buzzer = 8;
int vibra_analog = 0;
int estado_share = 0;
int posicao_camera = 90;
int camera_parada = 90;
int R2,L2,Move,Camera;

bool vibra = false;
bool botao_sport = false;
bool freio_mao = false;
bool direcao = false;
bool connect = false;
bool lanterna = false;

char mac[] = "d8:9c:67:dd:b4:aa";

long tempoInicial;
long tempoAtual;

unsigned long tempoAnterior = 0; 
unsigned long tempo_posicao = 0;
const long PERIODO = 500;

void setColor(String cor);

void setup() {

 Serial.begin(115200);

 pinMode(m1_horario,OUTPUT);
 pinMode(m2_horario,OUTPUT);
 pinMode(m1_antihorario,OUTPUT);
 pinMode(m2_antihorario,OUTPUT);
 pinMode(led_1,OUTPUT);
 pinMode(led_2,OUTPUT);
 pinMode(buzzer,OUTPUT);
 
 digitalWrite(m1_horario,LOW);
 digitalWrite(m2_horario,LOW);
 digitalWrite(m1_antihorario,LOW);
 digitalWrite(m2_antihorario,LOW);
 digitalWrite(led_1,LOW);
 digitalWrite(led_2,LOW);
 digitalWrite(buzzer,LOW);

 servo1.attach(servoPin,3);
 servo1.write(camera_parada);

 ledcSetup(canal_R2, freq, resolution);
 ledcAttachPin(motor_1, canal_R2);
 ledcWrite(canal_R2, 0);

 ledcSetup(canal_L2, freq, resolution);
 ledcAttachPin(motor_2, canal_L2);
 ledcWrite(canal_L2, 0);

 //ledcSetup(canal_buzzer, freq_buzzer, resolution_buzzer);
 //ledcAttachPin(buzzer, canal_buzzer);
 
  bool trava_conect = true;

  while(!PS4.isConnected()){
    
    if(trava_conect){
      trava_conect = false;      
      if(!PS4.begin(mac)){
        Serial.println("Inicializacao falhada");
        return;
      }  
    }
    delay(500);
  }

}

void loop() {
  

  if(PS4.isConnected()){
    connect = false;
    if(freio_mao){
      digitalWrite(m1_horario,LOW);
      digitalWrite(m2_horario,LOW);
      digitalWrite(m1_antihorario,LOW);
      digitalWrite(m2_antihorario,LOW);
      setColor("amarelo");
      ledcWrite(canal_R2, 0);
      ledcWrite(canal_L2, 0);
      PS4.setRumble(0, 0);
      PS4.sendToController();
    }
    else{
      
      Move = PS4.data.analog.stick.lx;
      R2 = PS4.data.analog.button.r2;
      L2 = PS4.data.analog.button.l2;

      if((Move < 15 && Move >=0) || (Move > -15 && Move <=0)){
        Move = 0;
        direcao = false;
      }

      //Direções

      if(R2 > 0 && L2 > 0){
        //Serial.println("parado");
        R2 = 0;
        L2 = 0; 
        digitalWrite(m1_horario,LOW);
        digitalWrite(m2_horario,LOW);
        digitalWrite(m1_antihorario,LOW);
        digitalWrite(m2_antihorario,LOW);
        
      }

      if(R2 > 0 && L2 == 0 && Move == 0){
        //Serial.println("avanca");
        digitalWrite(m1_horario,HIGH);
        digitalWrite(m2_horario,HIGH);
        digitalWrite(m1_antihorario,LOW);
        digitalWrite(m2_antihorario,LOW);
        
      }

      if(L2 > 0 && R2 == 0 && Move == 0){
        //Serial.println("recua");

        digitalWrite(m1_horario,LOW);
        digitalWrite(m2_horario,LOW);
        digitalWrite(m1_antihorario,HIGH);
        digitalWrite(m2_antihorario,HIGH);
        
      }

      if(Move > 15){
        //Serial.println("direita");
        R2 = 0;
        L2 = 0;
        direcao = true;

        digitalWrite(m1_horario,LOW);
        digitalWrite(m2_horario,HIGH);
        digitalWrite(m1_antihorario,HIGH);
        digitalWrite(m2_antihorario,LOW);

      }
    

      if(Move < -15){
        //Serial.println("esquerda");
        R2 = 0;
        L2 = 0;
        direcao = true;

        digitalWrite(m1_horario,HIGH);
        digitalWrite(m2_horario,LOW);
        digitalWrite(m1_antihorario,LOW);
        digitalWrite(m2_antihorario,HIGH);
      }
      

      if(R2 == 0 && L2 == 0 && Move == 0){
        
        if(botao_sport){
          setColor("vermelho");
        }
        else{
          setColor("azul");
        }
        ledcWrite(canal_R2, 0);
        ledcWrite(canal_L2, 0);
        
        PS4.setRumble(0, 0);
        PS4.sendToController();
        delay(10);
      }
      else{
        
        if(botao_sport){
          
          //PS4.setRumble(PS4.data.analog.button.r2, PS4.data.analog.button.l2);
          //PS4.setLed(R2,L2,0);
          //PS4.sendToController();

          if(Move > 15){
            Move = map(Move, 15, 127, 0, velocidade_sport);
          }
          if(Move < -15){
            Move = map(Move, -15, -128, 0, velocidade_sport);
          }

          R2 = map(R2, 0, 255, 0, velocidade_sport);
          L2 = map(L2, 0, 255, 0, velocidade_sport);
          
          setColor("vermelho");
          
          if(direcao){
            if(vibra){
              vibra_analog = map(Move, 0, velocidade_sport, 0, 255);
              PS4.setRumble(vibra_analog, vibra_analog);
              PS4.sendToController();
            }
            else{
              PS4.setRumble(0, 0);
              PS4.sendToController();
            }
            ledcWrite(canal_R2, Move);
            ledcWrite(canal_L2, Move);
          }
          else{
            
            if(L2 == 0){
              if(vibra){
                vibra_analog = map(R2, 0, velocidade_sport, 0, 255);
                PS4.setRumble(vibra_analog, vibra_analog);
                PS4.sendToController();
              }
              else{
                PS4.setRumble(0, 0);
                PS4.sendToController();
              }
              ledcWrite(canal_R2, R2);
              ledcWrite(canal_L2, R2);
            }
            if(R2 == 0){
              if(vibra){
                vibra_analog = map(L2, 0, velocidade_sport, 0, 255);
                PS4.setRumble(vibra_analog, vibra_analog);
                PS4.sendToController();
              }
              else{
                PS4.setRumble(0, 0);
                PS4.sendToController();
              }
              ledcWrite(canal_R2, L2);
              ledcWrite(canal_L2, L2);
            }
            
          }
          
        }
        else{
          
          if(Move > 15){
            Move = map(Move, 15, 127, 0, velocidade_normal);
          }

          if(Move < -15){
            Move = map(Move, -15, -128, 0, velocidade_normal);
          }
          
          R2 = map(R2, 0, 255, 0, velocidade_normal);
          L2 = map(L2, 0, 255, 0, velocidade_normal);
          
          setColor("azul");

          if(direcao){
            if(vibra){
                vibra_analog = map(Move, 0, velocidade_normal, 0, 127);
                PS4.setRumble(vibra_analog, vibra_analog);
                PS4.sendToController();
              }
              else{
                PS4.setRumble(0, 0);
                PS4.sendToController();
              }
            ledcWrite(canal_R2, Move);
            ledcWrite(canal_L2, Move);
          }
          else{
            if(L2 == 0){
              if(vibra){
                vibra_analog = map(R2, 0, velocidade_normal, 0, 127);
                PS4.setRumble(vibra_analog, vibra_analog);
                PS4.sendToController();
              }
              else{
                PS4.setRumble(0, 0);
                PS4.sendToController();
              }
              ledcWrite(canal_R2, R2);
              ledcWrite(canal_L2, R2);
            }

            if(R2 == 0){
              if(vibra){
                vibra_analog = map(L2, 0, velocidade_normal, 0, 127);
                PS4.setRumble(vibra_analog, vibra_analog);
                PS4.sendToController();
              }
              else{
                PS4.setRumble(0, 0);
                PS4.sendToController();
              }
              ledcWrite(canal_R2, L2);
              ledcWrite(canal_L2, L2);
            }
          }
        }   
      }

      if(PS4.data.button.touchpad){
        
        delay(300);
        
        if(botao_sport){
          botao_sport = false;
        }
        else{
        botao_sport = true;
        }

      }

      /*
      tempoAtual = millis();

      if (tempoAtual > (tempoInicial + 990))
      {
        
        Serial.print("Move: ");
        Serial.println(Move);
        Serial.print("R2: ");
        Serial.println(R2);
        Serial.print("L2: ");
        Serial.println(L2);

        tempoInicial = tempoAtual; // somente se quiser resetar o tempo
      } 
      */
    }

    if(PS4.data.button.r3){
      delay(300);
      
      if(freio_mao){
        freio_mao = false;
      }
      else{
        freio_mao = true;
      }
    }

    if(PS4.data.button.options){
      delay(300);
      if(vibra){
        vibra = false;
      }
      else{
        vibra = true;
      }
    }

    if(PS4.data.button.l3){
      //ledcWriteTone(canal_buzzer,640);
      digitalWrite(buzzer,HIGH);    
    }
    else{
      //ledcWriteTone(canal_buzzer, 0);
      digitalWrite(buzzer,LOW);
    }

    if(PS4.data.button.r1){
      delay(300);
      if(lanterna){
        lanterna = false;
      }
      else{
        lanterna = true;
      }
    }

    if(PS4.data.button.share){

      if(estado_share == 0){
        estado_share = 1;
      }

    }
    else{
      estado_share = 0;
    }
   
    if(estado_share == 1){
      
      estado_share = 2;
      
      if(R2 == 0 && L2 == 0 && Move == 0){
        
      }
    }

    Camera = PS4.data.analog.stick.ry;
    
    //Serial.println(Camera);

    if(Camera > 15){
      //Serial.print("up");
      Camera = map(Camera, 15, 127, 90, 180);
      //Serial.println(Camera);
      unsigned long tempoAtual = millis();

      if (tempoAtual - tempo_posicao >= PERIODO) {
        if(posicao_camera < Camera){
          posicao_camera++;
          servo1.write(posicao_camera);
        }        
      }
    }

    if(Camera < -15){
      
      Camera = map(Camera, -15, -128, 90, 0);
      
      unsigned long tempoAtual = millis();

      if (tempoAtual - tempo_posicao >= PERIODO) {
        
        if(posicao_camera > Camera){
          posicao_camera--;
          servo1.write(posicao_camera);
        }   
      }
    }

    if(lanterna){
      digitalWrite(led_1,HIGH);
      digitalWrite(led_2,HIGH);
    }
    else{
      digitalWrite(led_1,LOW);
      digitalWrite(led_2,LOW);
    }
    
  }
  else{
    
    R2 = 0;
    L2 = 0;
    Move = 0;
    Camera = 0; 

    digitalWrite(m1_horario,LOW);
    digitalWrite(m2_horario,LOW);
    digitalWrite(m1_antihorario,LOW);
    digitalWrite(m2_antihorario,LOW);
    digitalWrite(led_1,LOW);
    digitalWrite(led_2,LOW);
    digitalWrite(buzzer,LOW);
    
    unsigned long tempoAtual = millis();

    if (tempoAtual - tempo_posicao >= PERIODO) {
      if(posicao_camera < camera_parada){
        posicao_camera++;
        servo1.write(posicao_camera);
      }        
    }

    if (tempoAtual - tempo_posicao >= PERIODO) {
      if(posicao_camera > camera_parada){
        posicao_camera--;
        servo1.write(posicao_camera);
      }   
    }

    if(connect){
    }
    else{

      if(!PS4.begin(mac)){
        Serial.println("Inicializacao falhada");
        return;
      }  
      connect = true;
    }
    
  }

  delay(10);
}


void setColor(String cor)
{

  if(cor.equals("red") || cor.equals("vermelho") ){
    PS4.setLed(255,0,0);
  }

  if(cor.equals("green") || cor.equals("verde") ){
    PS4.setLed(0,255,0);
  }

  if(cor.equals("blue") || cor.equals("azul") ){
    PS4.setLed(0,0,255);
  }

  if(cor.equals("yellow") || cor.equals("amarelo") ){
    PS4.setLed(255,255,0);
  }

  if(cor.equals("purple") || cor.equals("roxo") ){
    PS4.setLed(80,0,80);
  }
  PS4.sendToController();
}