#include <TimerOne.h>


#define PIN_BTNPLAY_PAUSE           12       // Defino el pin donde conecto el boton de start/pause.
#define PIN_BTNRESET                2      // Defino el pin donde conecto el boton de stop.

#define BOUD_RATE                   9600

#define US_TIMERONE                 1000

#define ESTADO_PAUSE                0
#define ESTADO_PLAY                 1
#define ESTADO_RESET                2

#define ESTADO_BOTON_ESPERA         0
#define ESTADO_BOTON_CONFIRMACION   1
#define ESTADO_BOTON_LIBERACION     2

#define ESTADO_BOTON_INICIAL    ESTADO_BOTON_ESPERA
#define MS_ANTIRREB 25  // Espera (en milisegundos) para el antirrebote

#define CANT_BOTONES                2

#define BTNPLAY_PAUSE               0
#define BTNRESET                    1

int estadoCronometro = ESTADO_PLAY;

bool flagContar = 1;
bool flagHs = false;

int ms, segs, mins, hs;

int estadoBoton[CANT_BOTONES];    // Array de estados actuales de las maquinas de estados de los botones

char flagBoton[CANT_BOTONES];   // Banderas que indican el estado actual de los botones. Los uso en la maq de estados
char pinBoton[CANT_BOTONES];    // Array de pines donde estan conectados los botones. Los uso en la maq de estados

int msBoton[CANT_BOTONES];      // Contadores de milisegundos para los antirrebotes de los botones

void timer (void);
void maquinaCronometro (void);
void FSM_Antirrebote(int boton);

void setup() {
  // put your setup code here, to run once:
  
  pinMode (PIN_BTNPLAY_PAUSE, INPUT_PULLUP);
  pinMode (PIN_BTNRESET, INPUT_PULLUP);
  pinBoton[BTNPLAY_PAUSE] = PIN_BTNPLAY_PAUSE;
  pinBoton[BTNRESET] = PIN_BTNRESET;

  Timer1.initialize(US_TIMERONE);
  Timer1.attachInterrupt(timer);
  
  Serial.begin (BOUD_RATE);

  estadoBoton[BTNPLAY_PAUSE] = ESTADO_BOTON_INICIAL;
  estadoBoton[BTNRESET] = ESTADO_BOTON_INICIAL;

  flagBoton[BTNPLAY_PAUSE] = 0;
  flagBoton[BTNRESET] = 0;

  msBoton[BTNPLAY_PAUSE] = 0;
  msBoton[BTNRESET] = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  
  maquinaCronometro();
  FSM_Antirrebote(BTNPLAY_PAUSE);
  FSM_Antirrebote(BTNRESET);

  if (flagHs == true)
  {
    Serial.print(hs);
    Serial.print(" : ");
    Serial.print(mins);
    Serial.print (" : ");
    Serial.print(segs);
    Serial.print(" : ");
    Serial.println(ms);
  }

  else
  {
    Serial.print(mins);
    Serial.print (" : ");
    Serial.print(segs);
    Serial.print(" : ");
    Serial.println(ms);
  }
  
}

void FSM_Antirrebote(int boton)
{
    char estadoPin;
    
    switch(estadoBoton[boton])
    {
      case ESTADO_BOTON_ESPERA:
                              estadoPin = digitalRead(pinBoton[boton]);
                              
                              // Si se da la condicion de cambio de estado
                              if( estadoPin == 0)
                              {
                                msBoton[boton] = 0;
                                estadoBoton[boton] = ESTADO_BOTON_CONFIRMACION;
                              }
      break;
          
      case ESTADO_BOTON_CONFIRMACION:
                                    estadoPin = digitalRead(pinBoton[boton]);
                                    
                                    // Si se da la condicion, se considera el boton como presionado
                                    if( estadoPin == 0 && msBoton[boton] >= MS_ANTIRREB)
                                    {
                                      estadoBoton[boton] = ESTADO_BOTON_LIBERACION;
                                    }
                        
                                    // Si se da la condicion, se considera ruido
                                    if( estadoPin == 1 && msBoton[boton] < MS_ANTIRREB)
                                    {
                                      estadoBoton[boton] = ESTADO_BOTON_ESPERA;
                                    }
      break;
          
      case ESTADO_BOTON_LIBERACION:
                                  estadoPin = digitalRead(pinBoton[boton]);
                                  
                                  // Si se da la condicion, se considera el boton como presionado
                                  if( estadoPin == 1)
                                  {
                                    flagBoton[boton] = 1;
                                    estadoBoton[boton] = ESTADO_BOTON_ESPERA;
                                  }
      break;
    }
}

void maquinaCronometro (void)
{
  switch (estadoCronometro)
  {
    case ESTADO_PAUSE:
                      if (flagBoton[BTNPLAY_PAUSE] == 1) //paso a PLAY
                      {
                        flagBoton[BTNPLAY_PAUSE] = 0;
                        flagContar = 1;
                        estadoCronometro = ESTADO_PLAY;
                      }

                      if (flagBoton[BTNRESET] == 1) // paso a RESET
                      {
                        flagBoton[BTNRESET] = 0;
                        ms = 0;
                        segs = 0;
                        mins = 0;
                        hs = 0;
                        flagHs = false;
                        estadoCronometro = ESTADO_RESET;
                      }
    break;

    case ESTADO_PLAY:
                    if (flagBoton[BTNPLAY_PAUSE] == 1) // paso a PAUSE
                    {
                      flagBoton[BTNPLAY_PAUSE] = 0;
                      flagContar = 0;
                      estadoCronometro = ESTADO_PAUSE;
                    }

                    if (flagBoton[BTNRESET] == 1) // paso a RESET
                      {
                        flagBoton[BTNRESET] = 0;
                        ms = 0;
                        segs = 0;
                        mins = 0;
                        hs = 0;
                        flagHs = false;
                        estadoCronometro = ESTADO_RESET;
                      }
    break;

    case ESTADO_RESET:
                      if (flagContar == 1) // significa que el estado anterior era PLAY
                      {
                        estadoCronometro = ESTADO_PLAY;
                      }

                      if (flagContar == 0) // significa que el estado anterior era PAUSE
                      {
                        estadoCronometro = ESTADO_PAUSE;
                      }
                      
    break;
  }
}

void timer (void)
{
  msBoton[BTNRESET] = msBoton[BTNRESET] + 1;
  msBoton[BTNPLAY_PAUSE] = msBoton[BTNPLAY_PAUSE] + 1;
    
    if (flagContar == 1)
    {
        ms = ms + 1;
    }
    
    if (ms >= 1000)
    {
      ms = 0;
      segs = segs + 1;
    }
  
    if (segs >= 60)
    {
      segs = 0;
      mins = mins + 1;
    }

    if (mins >= 60)
    {
      mins = 0;
      hs = hs + 1;
      flagHs = true;
    }

}
