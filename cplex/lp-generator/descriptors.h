#include <limits.h>
#define INF    INT_MAX

#ifndef GLOBAL
#define EXTERN extern
#else
#define EXTERN
#endif

struct	_Arc{
	int id;
	int e1;
	int e2;
	float dist;
	int classification;
	int periods;
};
typedef struct _Arc Arc;

struct _Node{
	int id;				//id of a node
	int status;			//0=origin, 1=destination, 2=transhipment
	int data;			//if origin the data is the number of equipemens, if destination the data is the number of population, if transhipment the data is 0
};
typedef struct _Node Node;

// general functions
void aloca_e_inicializa(char VerFile[], char EdgFile[]);
void	desaloca ();

// functions to generate the model 
void    objective_function ();			 //first objective function for maximizing the attended population in a smaller possible time
void	shortest_paths_constraints();	 //shortest path calculation
void	xz_constraints();				 //allows flow on an arc only if it's not blocked
void	flow_constraints ();			 //flow restrictions
void	date_constraints();				 //determine the date a node is visited
void	linking_variables_constraints(); //linking variables x_ij and f_ij
void	wtflow();
void	z_init_end();
void	repair();
void	yz_constraints();
void	classification();
void	variables_definition ();		

//----------------------------------------------------------------------------

EXTERN FILE *outputFile;

// Variables correspondantes aux données du problème
EXTERN int  nb_nodes;		  // number of nodes 
EXTERN int  nb_arcs;          // number of edges
EXTERN int  nb_origins;       // number of origins
EXTERN int  nb_destinations;  // number of destinations
EXTERN int  nb_equipements;	  // number of equipements
EXTERN int  nb_periods;       // number of periods 
EXTERN int  nb_blocked;       // number of blocked edges
EXTERN Arc  *arcs;			  // List of nodes
EXTERN Node *nodes;		      // List of nodes



 
