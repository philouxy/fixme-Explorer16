//-----------------------------------------------------------------------------------//
// Nom du projet 		:	Conv_ADC
// Nom du fichier 		:   main_adc.c
// Date de création 	:   23.08.2013
// Date de modification : 	22.10.2013
// 
// Auteur 				: 	Philou (Ph Bovey)
//
// Description 			: 	Ce programme doit afficher la tension au borne du potentiomètre 
//							sur l'écran LCD de la carte explorer 16
//
// Modification 		: - 
//
// Remarques			: 	
// 	chemin pour trouver le headerfile 
//		C:\Program Files\Microchip\MPLAB C30\support\dsPIC33F\h
//
//	doc pour le DSP : 
//		http://www.microchip.com/wwwproducts/Devices.aspx?dDocName=en546064
//  doc pour la carte explorer : 
//     	http://www.microchip.com/stellent/idcplg?IdcService=SS_GET_PAGE&nodeId=1406&dDocName=en024858&part=dm240001&redirects=explorer16
//----------------------------------------------------------------------------------//
//--- librairie à inclure ---// 
#if defined(__dsPIC33F__)
#include "p33Fxxxx.h"
#elif defined(__PIC24H__)
#include "p24Hxxxx.h"
#endif

//#include <stdbool.h>
//#include "antirebond.h"
#include "lcd.h"
//#include <math.h>

//--- configuration des fusibles ---//
_FOSCSEL(FNOSC_PRI); 								// utilisation du quartz de 8MHz externe 
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF  & POSCMD_XT); 	// pin OS2 garder pour l'horlohe 
													// oscillateur XT	
_FWDT(FWDTEN_OFF); 									// ne pas activer le watchdog


//--- déclaration de prototype de fonction ---//
void init_in_out(void); 							// configuration des entrées - sorties 
void init_adc(void); 								// configuration de l'ADC
void init_osci(void);								// configuration de l'oscillateur
void init_timer1(void);								// configuration du Timer 1  
void msg_acceuil(void);
void conv_val(int val_recu, char *pt_tb_val_ADC);	// fonction permettant de décomposant un nombre (millier, centaine,....)  
void msg_lcd(char *pt_tb_val_ADC);							// permet d'afficher une valeur  


//--- variable globale ---// 
int info_interrupt_ADC;				// info ninterruption du convertisseur 
int auto_affichage_val_ADC; 		// info donner par l'interruption lié au Timer 1 
int valeur_adc; 					// valeur tambon buffer ADC			

//--- programme principale ---// 
void main()
{
	//--- déclaration de variables ---// 
	char tb_val_ADC[4]; 					// variable de 8Bits 
	
	//int val_unit; 

	//int *pt_tb_val_ADC; 
	
	 

	//--- initialisation des variables ---//
	info_interrupt_ADC = 0;
	auto_affichage_val_ADC = 0; 

	//--- Initialisation ---// 
	init_in_out(); 				// initialisation des entrées/sorties
	init_osci(); 				// initialisation de l'oscillateur 
	init_adc(); 				// initialisation du convertisseur ADC
    init_timer1(); 				// initi	
	Init_LCD(); 				// initialisation de l'affichage 

	//--- message d'acceuil ---//
	msg_acceuil(); 

	while(1)
	{	
		//--- test si reçu le flag d'interruption lié à l'ADC ---//	
		if(info_interrupt_ADC == 1)
		{
			info_interrupt_ADC = 0; 				// remise à '0' du flag 
			conv_val(valeur_adc, &tb_val_ADC);  
		}
		
		//--- test si reçu le flag d'interruption ---//
		if(auto_affichage_val_ADC == 1)
		{
			auto_affichage_val_ADC = 0; 	// remise à '0' du flag 
			msg_lcd(&tb_val_ADC);
		}
	}
	//exit(0);
}



//----------------------------------------------------------------------------------//
//--- nom 				: init_in_out
//--- entrée - sortie 	: - / - 
//--- description 		: initilaiser les entrées et les sorties que l'on désire
//						: analogique ou numérique 
//----------------------------------------------------------------------------------//
void init_in_out(void)
{	
	//--- configuration entrée analogique ---// 
	AD1PCFGLbits.PCFG5 = 0; 						// port RB5/AN5 

	//--- configuration entrée numérique ---// 


	//--- configuration sortie numérique ---//  
}

//----------------------------------------------------------------------------------//
//--- nom 				: init_osci
//--- entrée - sortie 	: - / - 
//--- description 		: configure l'oscillateur du DSP pour utiliser l'horloge 
//						  à 40MHz
//----------------------------------------------------------------------------------//
void init_osci(void)
{
	//--- configuration de l'oscillateur interne à 40Mhz ---//
	// utilisation fomrules du datasheet du DSPic33FJ256GP710A --> page 146
	// Fcy = Fosc / 2 ; Fosc = Fin(M/(N1 N2))
	// Fin = 8Mhz --> Fosc = 80Mhz --> Fcy = 40Mhz 
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
	T1CONbits.TCKPS = 0x03; 	// prédivseur régler à 256 --> 40Mhz/256 = 156250Hz --> 6.4us 
	
	TMR1 = 0; 					// mise à zéro du registre lié au timer 

	PR1 = 15625; 				// période du timer --> 100ms/6.4us = 15625  

	//--- registres des configurations des interruptions liées au Timer1 ---//
	IPC0bits.T1IP = 7; 		// choix de la priorité --> ici la plus élevée 7 = 111
	IFS0bits.T1IF = 0; 			// remise à zéro de l'interruption 
	IEC0bits.T1IE = 1; 			// activation de l'interruption lié au Timer1 

	T1CONbits.TON = 1; 			// activer le timer1
}


//----------------------------------------------------------------------------------//
//--- nom 				: _T1Interrupt
//--- entrée - sortie 	: - / - 
//--- description 		: appel la fonction d'affichage par rapport a l'affichage 
//						  concerant le potentiomètre analogique 
//						  no_auto_psv
//----------------------------------------------------------------------------------//
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt( void )
{
	IFS0bits.T1IF = 0; 				// remise à zéor du flag d'interruption 
 
	auto_affichage_val_ADC = 1; 	// autorisation dans le main d'affichage la valeur     
	
	TMR1 = 0; 						// remise à zéro du compteur 
	T1CONbits.TON = 1;				// activer à nouveau le Timer 
}

//----------------------------------------------------------------------------------//
//--- nom 				: init_adc
//--- entrée - sortie 	: - / - 
//--- description 		: initilaiser les convertisseurs du DSPic   
//----------------------------------------------------------------------------------//
void init_adc()
{
	//--- gestion des registres liés au ADC ---//
	//--- Sélection du channal --> bits de 0 à 4 et bit 7---// 
	AD1CHS0bits.CH0SA = 0x05; 		// selection de l'entrée analogique sur le chanel 0 (mesure)  
	AD1CHS0bits.CH0NA = 0; 			// selection de la référence Vref- (masse) 

	//--- Choix du type de convertion (12/10 bits) -->bit 10 du registre ---//
	AD1CON1bits.AD12B = 0; 			// convertisseur sur 10 bits --> 0 
									// convertisseur sur 12 bits --> 1
	
	//--- Choix du format pour le buffer --> bit 8 à 9 ---// 
	AD1CON1bits.FORM = 0; 			// format Unsigned Interger (0 - 1023 or 0 - 4095)
	
	//--- Choix de la source pour le temps d'échantillonage (clock) --> bit 5 à 7 ---// 
	AD1CON1bits.SSRC = 0x07; 			// 0b111 = 7 --> Counter interne Automatic Trigger

	//--- choix de l'échantillonage --> bit 3 --// 
	AD1CON1bits.SIMSAM = 0; 		// ici qu'un seul canal
	
	//--- mode d'échantillonage --> bit 2 ---// 
	AD1CON1bits.SAMP = 0;
	AD1CON1bits.ASAM = 1; 			// converstion après la dernière converstion  

	//--- Choix de la référence des tension --> 3x bits de 13 à 15 ---//
	AD1CON2bits.VCFG = 0; 			// Vref + = AVdd = 5V ou 3V & Vref - = AVss = GNS  
	 
	//--- Choix des canaux (simple - deux - multiple) --> 2 bits de 8 et 9 ---//	
	AD1CON2bits.CHPS = 0; 			// seulement un chanal = CH0
	  
	//--- Configuration de la vitesse de l'interruption après échnatillonnage --> bit 2 à 5 ---// 
	AD1CON2bits.SMPI = 0;			// genère une interrruption après échantillonnage  

	//--- Temps automatic d'échantillonage --> 5x bit de 8 à 12 ---//
	AD1CON3bits.SAMC = 0x03; 		// 3 TAD

	//--- choix de la vitesse de conversion selon la clock choisie --> 8x bits de 0 à 7---// 
	AD1CON3bits.ADCS = 0x27;		// 0b 0010 0111 = 0x27 = 39  
	 								// 1 / Fcy = Tcy  
	 								// ADCX + 1 = x
	 								// Tcy * x = Tad = 1us  
									// 40MHz --> 25ns => 25ns * 40 = 1us

	//--- gestion de l'interruption ---// 
	//--- reset du flag d'interruption lié à l'AD ---// 
	IFS0bits.AD1IF = 0;				// remise à zéro 

	//--- priorité de l'interruption ---// 				
	IPC3bits.AD1IP = 0x07; 			// ici level le plus haut --> 7  	

	//--- activation de l'interruption ---// 
	IEC0bits.AD1IE = 1; 

	//--- activation du convertisseur ADC ---// 
	AD1CON1bits.ADON = 1; 			// convertisseur actif	 
}


//----------------------------------------------------------------------------------//
//--- nom 				: _ADC1Interrupt
//--- entrée - sortie 	: - / - 
//--- description 		: lors de l'interruption lié au niveau du convertisseur 
//						  recharger l'interruption...   
//----------------------------------------------------------------------------------//
void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt( void )
//void __attribute__((__interrupt__)) _ADC1Interrupt( void )
{
	//--- indication de l'interruption ---// 
	info_interrupt_ADC = 1;

	//--- lecture du registre pour récuperer la valeur l'entrée analogique ---// 
	valeur_adc = ADC1BUF0;			// lecture du buffeur

	//--- reset du flag d'interruption ---// 
	IFS0bits.AD1IF = 0;	
}


//----------------------------------------------------------------------------------//
//--- nom 				: msg_acceuil
//--- entrée - sortie 	: - / - 
//--- description 		: initilaiser les convertisseurs du DSPic   
//----------------------------------------------------------------------------------//
void msg_acceuil(void)
{
	//--- Reset du LCD ---//
	lcd_cmd(0x01); 					// #define home_clr() --> lcd_cmd( 0x01 )
									// #define cursor_right()  lcd_cmd( 0x14 )
	//--- texte de test ---// 
	lcd_data('F');  				
	lcd_data('I');
	lcd_data('X');
	lcd_data('M');
	lcd_data('E');

	lcd_cmd(0xC0); 					//#define line_2()        lcd_cmd( 0xC0 )			
}

//----------------------------------------------------------------------------------//
//--- nom 				: msg_acceuil
//--- entrée - sortie 	: - / - 
//--- description 		: initilaiser les convertisseurs du DSPic   
//--- modification 		: le 22.10.2013
//----------------------------------------------------------------------------------//
void msg_lcd(char *pt_tb_val_ADC)
{
	//--- déclaration de la variable dans la fonction ---// 
	int i;
	int j = 1; 

	//--- traitement du tableau - valeur en hexa convertie en décimal ---//
	/*for(i = 3; i > 0; i--)
	{
		switch (pt_tb_val_ADC[i])
		{
			case 0xA :
				pt_tb_val_ADC[i] = 0; 
				pt_tb_val_ADC[i - 1] += 1;  	 
			break;
			case 0xB :
				pt_tb_val_ADC[i] = 1; 
				pt_tb_val_ADC[i - 1] += 1;  	 
			break;
			case 0xC :
				pt_tb_val_ADC[i] = 2; 
				pt_tb_val_ADC[i - 1] += 1;  	 
			break;
			case 0xD :
				pt_tb_val_ADC[i] = 3; 
				pt_tb_val_ADC[i - 1] += 1;  	 
			break;
			case 0xE :
				pt_tb_val_ADC[i] = 4; 
				pt_tb_val_ADC[i - 1] += 1;  	 
			break; 
			case 0xF :
				pt_tb_val_ADC[i] = 5; 
				do
				{
					if(pt_tb_val_ADC[i - j] == 0xF)
					{
						pt_tb_val_ADC[i - j] = 5; 
					}
					else 
						pt_tb_val_ADC[i - 1] += 1;
	
				} while (pt_tb_val_ADC[i - j] == 0xF);  	 
			break;
		} 
	} */

	//--- mettre le curseur sur la ligne 2 ---//
	lcd_cmd(0xC0); 

	//--- affichager la valeur du converstisseur ---//
	for(i = 0; i < 4 ; i++)
	{
		//--- definir le data ---//
		lcd_data(pt_tb_val_ADC[i] + 0x30);
		//pt_tb_val_ADC =+ i; 

		//--- definir la position ---//
		lcd_cmd(0xC0 + (i + 1));  					// déplacement du curseur à gauche
	}
}


//----------------------------------------------------------------------------------//
//--- nom 				: conv_val
//--- entrée - sortie 	: val_recu / *pt_tb_val_ADC 
//--- description 		: permet de découper une valeur par unité ; le but est de 
//						  récupérer chaque champs  
//--- date mode			: 16.10.2013   
//--- remarque 			: la valeur est reçu en hexa sur 16bits
//----------------------------------------------------------------------------------//
void conv_val(int val_recu, char *pt_tb_val_ADC)
{
	//--- déclaration de variables utilisée quand dans la fonction ---// 
	//--- constante ---// 
	const int mille = 1000; 
	const int cent = 100; 
	const int dix = 10; 

	//--- variable ---// 
	char bits_inf, bits_sup; 

	//int i = 0;
	*pt_tb_val_ADC = val_recu/mille;	
	val_recu %= mille; 
	pt_tb_val_ADC += 1;

	*pt_tb_val_ADC = val_recu/cent;	
	val_recu %= cent; 
	pt_tb_val_ADC += 1;

	*pt_tb_val_ADC = val_recu/dix;	
	val_recu %= 10; 
	pt_tb_val_ADC += 1;
	*pt_tb_val_ADC = val_recu;
 
	/*
	//--- récupération de la valeur supérieur sur 8bits ---// 
	bits_sup = (char)((val_recu >> 8) & 0x00FF);				// récupération des 8bits	 
	*pt_tb_val_ADC = (bits_sup >> 4) & 0x0F; 					// Enregistrement de la valeur à l'adresse 0 du tableau tb[0]
	pt_tb_val_ADC += 1; 										// incrémentation du pointeur   
	*pt_tb_val_ADC = bits_sup & 0x0F;							// récupération des 4bits inférieur + enregistrement à la valeur tb[1] 
	pt_tb_val_ADC += 1;											// incrémentation du pointeur

	//--- récupération de la valeur inférieur sur 8bits ---//	
    bits_inf = (char)(val_recu & 0x00FF);						// récupération des 8bits	 
	*pt_tb_val_ADC = (bits_inf >> 4) & 0x0F; 					// récupération des 4bits supérieur + enregsitrement de la valezr tb[2]
	pt_tb_val_ADC += 1; 										// incrémentation du pointeur 
	*pt_tb_val_ADC =  bits_inf & 0x0F;							// récupération des 4bits inférieur + enregsitrement de la valeur tb[3] 
	*/  
	/*
	if(i == 0)
	{	
		pt_tb_val_ADC += i; 
		*pt_tb_val_ADC = (char)(val_recu / mille);
		val_recu = val_recu % mille;  
		i++;  
	}
	if(i == 1)
	{	
		pt_tb_val_ADC += i;	
		*pt_tb_val_ADC = (char)(val_recu / cent);
		val_recu = val_recu % cent;  
		i++;		 
	}
	if(i == 2)
	{	
		pt_tb_val_ADC += i;
		*pt_tb_val_ADC  = (char)val_recu / dix ;
		val_recu = val_recu % dix;  
		i++; 
	}
	if(i == 3)
	{	
		pt_tb_val_ADC += i;
		*pt_tb_val_ADC = (char)val_recu ;
		i = 0;	 
	}*/
 
}








/*
//----------------------------------------------------------------------------------//
//--- nom 				: init_timer1
//--- entrée - sortie 	: - / - 
//--- description 		: initilaiser le timer 1 pour avoir une horloge de 5ms 
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
*/

/*
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
*/








 	
/*
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

*/