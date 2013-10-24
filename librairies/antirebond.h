//---------------------------------------------------------------------------------------//
// Nom du projet 		:	LED_CLIGNO_1B
// Nom du fichier 		:   antirebond.h
// Date de création 	:   24.05.2013
// Date de modification : 	xx.xx.2012
// 
// Auteur 				: 	Philou - Ph.Bovey 
//
// Description 			: 	Header file permettant d'utiliser les fonctions d'antirebond 

// Remarques			: 	
// 	chemin pour trouver le headerfile 
//		C:\Program Files\Microchip\MPLAB C30\support\dsPIC33F\h
//
//	doc pour le DSP : 
//		http://www.microchip.com/wwwproducts/Devices.aspx?dDocName=en546064
//---------------------------------------------------------------------------------------//
#ifndef ANTIREBOND_H
#define ANTIREBOND_H

//--- déclaration d'enumeration ---//
typedef enum
{
	Attente_changement_touche, 		
	Attente_stabilite_touche, 
}Attente_Etat;

//--- déclaration de strcutures ---//
typedef struct
{
	Attente_Etat Etat_antirepond;
	int compteur; 
	int T_presse; 
	int T_relache; 
	int val_T_presendante; 
	int val_T; 
}Etat_switch; 

//--- déclaration des protoytes ---//
extern void antirebond(Etat_switch *p_st_Etat, int val_in); 
extern void init_antirebond(Etat_switch *p_st_Etat_init); 
extern int etat_switch_apres_rebond(Etat_switch *p_st_Etat); 
extern int switch_presser(Etat_switch *p_st_Etat); 
extern int switch_relacher(Etat_switch *p_st_Etat); 
extern void reset_info_touche_presser(Etat_switch *p_st_Etat); 
extern void reset_info_touche_relacher(Etat_switch *p_st_Etat); 

#endif