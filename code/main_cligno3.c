//-----------------------------------------------------------------------------------//
// Nom du projet 		:	LED_CLIGNO_1B
// Nom du fichier 		:   main.c
// Date de création 	:   18.12.2012
// Date de modification : 	03.07.2013
// 
// Auteur 				: 	Philou (Ph Bovey)
//
// Description 			: 	Ce programme doit faire clignoter une led, et si l'on 
//							appuie sur un switch (S3 - Carte Explorer16), c'est une 
//							autre led qui clignote
//							+ gestion de l'antirebond 
// Remarques			: 	
// 	chemin pour trouver le headerfile 
//		C:\Program Files\Microchip\MPLAB C30\support\dsPIC33F\h
//
//	doc pour le DSP : 
//		http://www.microchip.com/wwwproducts/Devices.aspx?dDocName=en546064
//----------------------------------------------------------------------------------//
//--- librairie à inclure ---// 
#if defined(__dsPIC33F__)
#include "p33Fxxxx.h"
#elif defined(__PIC24H__)
#include "p24Hxxxx.h"
#endif

//#include <stdbool.h>
#include "antirebond.h"

//--- configuration des fusibles ---//
_FOSCSEL(FNOSC_PRI); 								// utilisation du quartz de 8MHz externe 
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF  & POSCMD_XT); 	// pin OS2 garder pour l'horlohe 
													// oscillateur XT	
_FWDT(FWDTEN_OFF); 									// ne pas activer le watchdog

//--- prototype de fonction ---//
void init_in_out(void); 							// configuration des entrées - sorties 
void init_osci(void); 
void init_timer1(void); 							// configuration du timer1; 
void leds_light(void);								// permet d'allumer toutes les leds 
void Leds_Cli(int num_led); 						// permet de faire clgnoter une leds selon un paramètre 

//--- déclaration de variable globale ---//
int indice_led;										// indice pour le choix du clignotement de la LED
int lecture_S_Avant = 0; 							// indice pour la touche (AVANT delay)					
int lecture_S_Apres = 0; 							// indice pour la touche (APRES delay)
int compteur = 0;

Etat_switch p_statut_S3; 
//---  
	
//--- programme principale ---//
void main()
{
	//--- déclaration de variables ---//
	//compteur = 0;									// initialisation du compteur  
	
	//--- initialisation de variables ---//
	indice_led = 0; 

	//--- désactiver le watchdog ---//
	RCONbits.SWDTEN = 0; 		
 
	//--- initialisation entrées - sorties ---//
	init_in_out(); 

	//--- initialisation de l'antirebond à la touche S3 (port RD6) ---// 
	init_antirebond(&p_statut_S3);

	//--- initialisation oscillateur ---//
	init_osci(); 

	//--- initialisation timer1 ---//
	init_timer1();

	//--- allumer toutes les leds ---//
	leds_light(); 

	//--- boucle sans fin ---//
	while(1)
	{
		if(switch_presser(&p_statut_S3))
		{
			indice_led++;
			reset_info_touche_presser(&p_statut_S3); 
		}
	}	
}


//--- déclaration de fonction ---//
//----------------------------------------------------------------------------------//
//--- nom 				: init_in_out
//--- entrée - sortie 	: - / - 
//--- description 		: initilaiser les entrées et les sorties que l'on désire
//						: analogique ou numérique 
//----------------------------------------------------------------------------------//
void init_in_out(void)
{	
	//--- configuration des entrées numériques ---//
	TRISDbits.TRISD6 = 1; 		// correspond à l'entrée S3

	//--- configuration des sorties numériques ---//
	TRISAbits.TRISA0 = 0; 
	TRISAbits.TRISA1 = 0; 
	TRISAbits.TRISA2 = 0; 
	TRISAbits.TRISA3 = 0; 
	TRISAbits.TRISA4 = 0; 
	TRISAbits.TRISA5 = 0;
	TRISAbits.TRISA6 = 0;
	TRISAbits.TRISA7 = 0; 
} 

//----------------------------------------------------------------------------------//
//--- nom 				: leds_light
//--- entrée - sortie 	: - / - 
//--- description 		: allume toutes les leds
//----------------------------------------------------------------------------------//
void leds_light(void)
{
	LATAbits.LATA0 = 1; 
	LATAbits.LATA1 = 1;
	LATAbits.LATA2 = 1;
	LATAbits.LATA3 = 1;
	LATAbits.LATA4 = 1;
	LATAbits.LATA5 = 1;
	LATAbits.LATA6 = 1;
	LATAbits.LATA7 = 1;
} 

//----------------------------------------------------------------------------------//
//--- nom 				: init_osci
//--- entrée - sortie 	: - / - 
//--- description 		: configure l'oscillateur du DSP pour utiliser l'horloge 
//						  à 40MHz
//----------------------------------------------------------------------------------//
init_osci()
{
	//--- configuration de l'oscillateur interne à 40Mhz ---//
	// utilisation fomrules du datasheet du DSPic33FJ256GP710A --> page 146
	// Fcy = Fosc / 2 ; Fosc = Fin(M/(N1 N2))
	// Fin = 8Mhz --> Fosc = 40Mhz --> Fcy = 40Mhz 
	OSCCONbits.COSC 	= 3;			// sélection de l'oscillateur XT 
	OSCCONbits.CLKLOCK	= 0;			// l'horloge et la PLL peuvent être modifié 	
	
	CLKDIVbits.ROI	= 0; 				// pas d'effet si il y a interruption 
	CLKDIVbits.DOZE	= 0; 				// pas de réduction sur l'horloge Fcy /1
	CLKDIVbits.DOZEN	= 0; 		
	CLKDIVbits.PLLPRE 	= 0; 			// N1 = 2	
    CLKDIVbits.PLLPOST 	= 0; 			// N2 = 2 	
	
	PLLFBDbits.PLLDIV 	= 38;			// M = 40  

	__builtin_write_OSCCONH(0x03); 		// fonction appelant du code assembleur 
										// configuration du registre NOSC (OSCCON) --> 
										// 011 = Primary Oscillator with PLL (XTPLL, HSPLL, ECPLL)
	__builtin_write_OSCCONL(0x01);		// Active la commutation de la clock
	while(OSCCONbits.COSC != 0b011); 	//
}

//----------------------------------------------------------------------------------//
//--- nom 				: init_timer1
//--- entrée - sortie 	: - / - 
//--- description 		: initilaiser le timer 1 pour avoir une horloge de 100ms 
//----------------------------------------------------------------------------------//
void init_timer1(void)
{
	//--- registre de configuration du Timer1 ---//
	T1CONbits.TON = 0; 			// désactiver le timer pour la configuration 
	T1CONbits.TCS = 0; 			// clock interne 
	T1CONbits.TSYNC = 0; 		// synchroniser la clock externe 	
	T1CONbits.TCKPS = 3; 		// prédivseur régler à 256 --> 40Mhz/256 = 156250Hz --> 6.4us 
	
	TMR1 = 0; 					// mise à zéro du registre lié au timer 

	PR1 = 781; 					// période du timer --> 5ms/6.4us = 781.25 = 781 

	//--- registres des configurations des interruptions liées au Timer1 ---//
	IPC0bits.T1IP = 7; 			// choix de la priorité --> ici la plus élevée 7 = 111
	IFS0bits.T1IF = 0; 			// remise à zéro de l'interruption 
	IEC0bits.T1IE = 1; 			// activation de l'interruption lié au Timer1 

	T1CONbits.TON = 1; 			// activer le timer1
}


//----------------------------------------------------------------------------------//
//--- nom 				: _T1Interrupt
//--- entrée - sortie 	: - / - 
//--- description 		: renvoit un numéro lié à la led qui doit clignoter 
//----------------------------------------------------------------------------------//
void __attribute__((interrupt,no_auto_psv)) _T1Interrupt( void )
{
	IFS0bits.T1IF = 0; 		// remise à zéor du flag d'interruption 

	//--- prend l'état inverse de l'état présédent ---//
	if(compteur >= 100)
	{
		Leds_Cli(indice_led);
		compteur = 0; 
	}
	else 
		compteur++; 

	//--- active la fonction d'antirebond à chaque interruption ---// 
	antirebond(&p_statut_S3, PORTDbits.RD6);
	
	//--- permet d'avoir une base de temps pour la lecture du switch ---//
	//compteur++;    	

	TMR1 = 0; 
	T1CONbits.TON = 1;		// activer à nouveau le Timer 
}

//----------------------------------------------------------------------------------//
//--- nom 				: Leds_Cli
//--- entrée - sortie 	: num_led / - 
//--- description 		: selon le numero en entrée cela change l'état d'une led 
//----------------------------------------------------------------------------------//
void Leds_Cli(int num_led)
{	
	
	switch(num_led)
	{
		//--- led D3 ---//
		case 0 :
			LATAbits.LATA0 = ~LATAbits.LATA0;  	// invserse la valeur pécédente  
			break;
		//--- led D4 ---// 
		case 1 :
			LATAbits.LATA1 = ~LATAbits.LATA1;  	// invserse la valeur pécédente  
			break; 
		//--- led D5 ---//
		case 2 :
			LATAbits.LATA2 = ~LATAbits.LATA2;  	// invserse la valeur pécédente  
			break;
		//--- led D6 ---// 
		case 3 :
			LATAbits.LATA3 = ~LATAbits.LATA3;  	// invserse la valeur pécédente  
			break;
		//--- led D7 ---// 
		case 4 :
			LATAbits.LATA4 = ~LATAbits.LATA4;  	// invserse la valeur pécédente  
			break; 	
		//--- led D8 ---//
		case 5 :
			LATAbits.LATA5 = ~LATAbits.LATA5;  	// invserse la valeur pécédente  	
			break; 
		//--- led D9 ---//
		case 6 :
			LATAbits.LATA6 = ~LATAbits.LATA6;  	// invserse la valeur pécédente  
			break; 
		//--- led D10 ---//
		case 7 :	
			LATAbits.LATA7 = ~LATAbits.LATA7;  	// invserse la valeur pécédente  
			break;

		default :
			indice_led = 0;
			LATAbits.LATA0 = 1;
			LATAbits.LATA1 = 1;
			LATAbits.LATA2 = 1;
			LATAbits.LATA3 = 1;
			LATAbits.LATA4 = 1;
			LATAbits.LATA5 = 1;
			LATAbits.LATA6 = 1;  
			LATAbits.LATA7 = 1; 
			break;  
	}
}