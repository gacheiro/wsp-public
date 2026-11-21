/*** Written by Celso Sakuraba 14/11/2014			  ***/
/*** Main procedure to generate .lp files for the WSP ***/
/*** This version still doesn't consider reverse arcs ***/

#define GLOBAL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>
#include <math.h>
#include <assert.h>
#include "descriptors.h"

//----------------------------------------------------------------------------
// Main program
// Input parameters
// 1. Input file

int main (int argc, char ** argv)
{
	int		i;
	char	vertex[64], edge[64], trash;
	FILE     *input;

	if (argc != 2)
	{
		printf("Arguments incorrect. Insert the name of a file containing a list of vertex and edge files.\nPress any key to close.\n");
		getchar();
		return(EXIT_FAILURE);
	}

	input = fopen(argv[1], "rt");
	if (input == NULL)
	{
		printf("unable to open %s\n", argv[1]);
		getchar();
		return(EXIT_FAILURE);
	}

	for (i = 0; i < 80; i++)
	{
		fscanf(input, "%[^ ]", vertex);
		fscanf(input, "%c", &trash);
		fscanf(input, "%[^\n]", edge);
		fscanf(input, "%c", &trash);

		aloca_e_inicializa(&vertex, &edge);  // Alocates and initialize data structures	
		objective_function();
		flow_constraints();
		linking_variables_constraints();
		wtflow();
		z_init_end();
		repair();
		yz_constraints();
		classification();
		xz_constraints();
		shortest_paths_constraints();
		variables_definition();
		desaloca();
	}
	
	return(0);
} //end_main


//----------------------------------------------------------------------------

// Aloca et initialise les structures de données
void aloca_e_inicializa(char VerFile[], char EdgFile[])
{
  char nomeOut[256] = ""; // output file
  int  i, position;
  float	trash;
  FILE *input;
  
  nb_equipements = 0;
  nb_periods = 0;
  nb_destinations = 0;
  nb_blocked = 0;

  input = fopen(VerFile, "r");
  if (input)
  {
	  fscanf(input, "%d", &nb_nodes);
	  nodes = (Node *)malloc(sizeof(Node)* (nb_nodes));
	  for (i = 0; i < nb_nodes; i++)
	  {
		  fscanf(input, "%d %f %f %d %d", &nodes[i].id, &trash, &trash, &nodes[i].status, &nodes[i].data);
		  if (nodes[i].status > 0 && nodes[i].status < 4)
			  nb_equipements += nodes[i].data;
		  else if (nodes[i].status == 4)
			  nb_destinations++;
	  }
  }
  else
  {
	  printf("Erro na abertura do arquivo %s.\n", input);
	  exit(0);
  }
  fclose(input);

  input = fopen(EdgFile, "r");
  if (input)
  {
	fscanf(input, "%d", &nb_arcs);
	arcs = (Arc *)  malloc(sizeof(Arc)* (nb_arcs));
	for (i = 0; i < nb_arcs; i++)
	{
		fscanf(input, "%d %d %d %f %d %d", &arcs[i].id, &arcs[i].e1, &arcs[i].e2, &arcs[i].dist, &arcs[i].classification, &arcs[i].periods);
		if (arcs[i].periods > 0)
		{
			nb_periods += arcs[i].periods;
			nb_blocked++;
		}
	}
  }
  else
  {
    printf("Erro na abertura do arquivo %s.\n", input);
    exit(0);
  }
  fclose(input);

  nb_periods = ceil ((double) nb_periods / nb_equipements);
//  nb_periods = 12;

  // output file name
  position = (int)strlen(EdgFile) - 4;
  strncpy(nomeOut, EdgFile, position);
  strcat(nomeOut, ".lp");

  if ((outputFile = fopen(nomeOut, "w")) == NULL)
  {
    printf("Problema de criacao do arquivo de saida\n");
  }//end_if

}//end_aloca

//----------------------------------------------------------------------------

// Desaloca les structures de données
void desaloca()
{
  free(nodes);
  free(arcs);
  fclose(outputFile);
} //fim_desaloca

//----------------------------------------------------------------------------
void objective_function(){
	int i, t, cont = 0;
	fprintf(outputFile, "min ");
	for (t = 1; t <= nb_periods; t++)
	{
		for (i = 0; i < nb_nodes; i++)
		{
			if (nodes[i].status == 4)
				fprintf(outputFile, " + %d s%d.%d", nodes[i].data, nodes[i].id, t);
		}//end_for
	}
	fprintf(outputFile, "\n\nst \n\n");
}//end_objective_function

//----------------------------------------------------------------------------

void flow_constraints()
{
	int  i, j, k;
	int  id, cont = 0;

	for (k = 1; k <= nb_periods; k++)
	{

		//---------------------------------------------------------
		//---artificial node
		//---------------------------------------------------------

		fprintf(outputFile, "c2.%d.0 :", k);

		for (j = 0; j < nb_nodes; j++)
		{
			if (nodes[j].status > 0 && nodes[j].status < 4)
				fprintf(outputFile, " + f0.%d.%d", nodes[j].id, k);
		}
		fprintf(outputFile, " = %d\n", nb_destinations);

		id = 1;

		//-----------------------------------------------------------------------------------
		//---flow conservation contraints
		//-----------------------------------------------------------------------------------
		for (j = 0; j < nb_nodes; j++)
		{
			if (nodes[j].status < 4)
			{
				fprintf(outputFile, "c2.%d.%d : ", k, id); id++;
				if (nodes[j].status > 0)
					fprintf(outputFile, "- f0.%d.%d ", nodes[j].id, k);

				for (i = 0; i < nb_arcs; i++)
				{
					if (arcs[i].e1 == nodes[j].id)
					{
						fprintf(outputFile, "+ f%d.%d.%d ", arcs[i].e1, arcs[i].e2, k);
						fprintf(outputFile, "- f%d.%d.%d ", arcs[i].e2, arcs[i].e1, k);
					}//end_if
				}//end_for

				for (i = 0; i < nb_arcs; i++)
				{
					if ((arcs[i].e2 == nodes[j].id))
					{
						fprintf(outputFile, "+ f%d.%d.%d ", arcs[i].e2, arcs[i].e1, k);
						fprintf(outputFile, "- f%d.%d.%d ", arcs[i].e1, arcs[i].e2, k);
					}//end_if
				}//end_for

				fprintf(outputFile, "= 0\n");
			}//end_if
		}//end_for_j

		//---------------------------------------------------------
		//---destination
		//---------------------------------------------------------

		for (j = 0; j < nb_nodes; j++)
		{
			if (nodes[j].status == 4)
			{
				fprintf(outputFile, "c2.%d.%d : ", k, id); id++;

				for (i = 0; i < nb_arcs; i++)
				{
					if (arcs[i].e1 == nodes[j].id)
					{
						fprintf(outputFile, "+ f%d.%d.%d ", arcs[i].e2, arcs[i].e1, k);
						fprintf(outputFile, "- f%d.%d.%d ", arcs[i].e1, arcs[i].e2, k);
					}//end_if
				}//end_for

				for (i = 0; i < nb_arcs; i++)
				{
					if ((arcs[i].e2 == nodes[j].id))
					{
						fprintf(outputFile, "+ f%d.%d.%d ", arcs[i].e1, arcs[i].e2, k);
						fprintf(outputFile, "- f%d.%d.%d ", arcs[i].e2, arcs[i].e1, k);
					}//end_if
				}//end_for

				fprintf(outputFile, "= 1\n", nb_destinations);
			}//end_for_if
		}//end_for_j
	}//end_for_k
	fprintf(outputFile, "\n");
}//flow_costraints
//----------------------------------------------------------------------------

void linking_variables_constraints()
{
	int  i, id, t;
	float aux = 1 / (float)nb_destinations;

	for (t = 1; t < nb_periods + 1; t++)
	{
		id = 1;
		for (i = 0; i < nb_arcs; i++)
		{
			fprintf(outputFile, "c3.%d.%d : x%d.%d.%d ", t, id, arcs[i].e1, arcs[i].e2, t);
			fprintf(outputFile, "- %lf f%d.%d.%d >= 0\n", aux, arcs[i].e1, arcs[i].e2, t);
			id++;
			fprintf(outputFile, "c3.%d.%d : x%d.%d.%d ", t, id, arcs[i].e2, arcs[i].e1, t);
			fprintf(outputFile, "- %lf f%d.%d.%d >= 0\n", aux, arcs[i].e2, arcs[i].e1, t);
			id++;
		}//end_for
	}
	fprintf(outputFile, "\n");

	for (t = 1; t < nb_periods + 1; t++)
	{
		id = 1;
		for (i = 0; i < nb_arcs; i++)
		{
			fprintf(outputFile, "c4.%d.%d : x%d.%d.%d ", t, id, arcs[i].e1, arcs[i].e2, t);
			fprintf(outputFile, "- f%d.%d.%d <= 0\n", arcs[i].e1, arcs[i].e2, t);
			id++;
			fprintf(outputFile, "c4.%d.%d : x%d.%d.%d ", t, id, arcs[i].e2, arcs[i].e1, t);
			fprintf(outputFile, "- f%d.%d.%d <= 0\n", arcs[i].e2, arcs[i].e1, t);
			id++;
		}//end_for
	}
	fprintf(outputFile, "\n");
}
//----------------------------------------------------------------------------

void wtflow()
{
	int  i, j, t;
	int  id;

	id = 1;
	for (t = 1; t <= nb_periods; t++)
	{
		for (i = 0; i < nb_nodes; i++)
		{
			if (nodes[i].status > 0 && nodes[i].status < 4)
			{
				fprintf(outputFile, "c5.%d.%d : ", t, id); id++;
				for (j = 0; j < nb_arcs; j++)
				{
					if (arcs[j].e1 == nodes[i].id)
						fprintf(outputFile, "+ y%d.%d.%d ", arcs[j].e1, arcs[j].e2, t);
					else if (arcs[j].e2 == nodes[i].id)
						fprintf(outputFile, "+ y%d.%d.%d ", arcs[j].e2, arcs[j].e1, t);
				}
				for (j = 0; j < nb_arcs; j++)
				{
					if (arcs[j].periods > 0)
					{
						if (arcs[j].e1 == nodes[i].id)
							fprintf(outputFile, "+ w%d.%d.%d ", arcs[j].e1, arcs[j].e2, t);
						else if (arcs[j].e2 == nodes[i].id)
							fprintf(outputFile, "+ w%d.%d.%d ", arcs[j].e2, arcs[j].e1, t);
					}
				}
				fprintf(outputFile, "<= %d\n", nodes[i].data);
			}
		}//end_for_i
	}
	fprintf(outputFile, "\n");

	id = 1;
	for (t = 1; t <= nb_periods; t++)
	{
		for (i = 0; i < nb_nodes; i++)
		{
			if (nodes[i].status == 0 || nodes[i].status == 4)
			{
				fprintf(outputFile, "c6.%d.%d : ", t, id); id++;
				for (j = 0; j < nb_arcs; j++)
				{
					if (arcs[j].e1 == nodes[i].id)
						fprintf(outputFile, "+ y%d.%d.%d - y%d.%d.%d ", arcs[j].e2, arcs[j].e1, t, arcs[j].e1, arcs[j].e2, t);
					else if (arcs[j].e2 == nodes[i].id)
						fprintf(outputFile, "+ y%d.%d.%d - y%d.%d.%d ", arcs[j].e1, arcs[j].e2, t, arcs[j].e2, arcs[j].e1, t);
				}
				for (j = 0; j < nb_arcs; j++)
				{
					if (arcs[j].periods > 0)
					{
						if (arcs[j].e1 == nodes[i].id)
							fprintf(outputFile, "- w%d.%d.%d ", arcs[j].e1, arcs[j].e2, t);
						else if (arcs[j].e2 == nodes[i].id)
							fprintf(outputFile, "- w%d.%d.%d ", arcs[j].e2, arcs[j].e1, t);
					}
				}
				fprintf(outputFile, "= 0\n");
			}//end_if
		}//end_for_i
	}
	fprintf(outputFile, "\n");
}
//----------------------------------------------------------------------------

void z_init_end()
{
	int i;
	fprintf(outputFile, "c7 : ");
	for (i = 0; i < nb_arcs; i++)
	{
		if (arcs[i].periods > 0)
			fprintf(outputFile, "+ z%d.%d.0 ", arcs[i].e1, arcs[i].e2);
	}
	fprintf(outputFile, "= 0\n\n");

/*	fprintf(outputFile, "c8 : ");
	for (i = 0; i < nb_arcs; i++)
	{
		if (arcs[i].periods > 0)
			fprintf(outputFile, "+ z%d.%d.%d ", arcs[i].e1, arcs[i].e2, nb_periods);
	}
	fprintf(outputFile, "= %d\n\n", nb_blocked);*/
}

void repair()
{
	int i, t, t1, id;
	for (t = 1; t <= nb_periods; t++)
	{
		id = 1;
		for (i = 0; i < nb_arcs; i++)
		{
			if (arcs[i].periods > 0)
			{
				fprintf(outputFile, "c9.%d.%d : ", t, id); id++;
				fprintf(outputFile, "+ %d z%d.%d.%d ", arcs[i].periods, arcs[i].e1, arcs[i].e2, t);
				for (t1 = 1; t1 <= t; t1++)
				{
					fprintf(outputFile, "- w%d.%d.%d ", arcs[i].e1, arcs[i].e2, t1);
					fprintf(outputFile, "- w%d.%d.%d ", arcs[i].e2, arcs[i].e1, t1);
				}
				fprintf(outputFile, "<= 0\n");
			}
		}
	}
	fprintf(outputFile, "\n");
}

void yz_constraints()
{
	int i, t, id;
	for (t = 1; t <= nb_periods; t++)
	{
		id = 1;
		for (i = 0; i < nb_arcs; i++)
		{
			if (arcs[i].periods > 0)
			{
				fprintf(outputFile, "c10.%d.%d : ", t, id); id++;
				fprintf(outputFile, "+ y%d.%d.%d ", arcs[i].e1, arcs[i].e2, t);
				fprintf(outputFile, "+ y%d.%d.%d ", arcs[i].e2, arcs[i].e1, t);
				fprintf(outputFile, "- %d z%d.%d.%d ", nb_equipements, arcs[i].e1, arcs[i].e2, t - 1);
				fprintf(outputFile, "<= 0\n");
			}
		}
	}
	fprintf(outputFile, "\n");
}

void classification()
{
	int i, t, id;
	for (t = 1; t <= nb_periods; t++)
	{
		id = 1;
		for (i = 0; i < nb_arcs; i++)
		{
			if (arcs[i].periods > 0)
			{
				fprintf(outputFile, "c11.%d.%d : ", t, id); id++;
				fprintf(outputFile, "w%d.%d.%d <= %d\n", arcs[i].e1, arcs[i].e2, t, arcs[i].classification);
				fprintf(outputFile, "c11.%d.%d : ", t, id); id++;
				fprintf(outputFile, "w%d.%d.%d <= %d\n", arcs[i].e2, arcs[i].e1, t, arcs[i].classification);
			}
		}
	}
	fprintf(outputFile, "\n");
}

void xz_constraints()
{
	int i, j, id = 1;
	for (i = 1; i <= nb_periods; i++)
	{
		for (j = 0; j < nb_arcs; j++)
		{
			fprintf(outputFile, "c12.%d.%d : s%d.%d ", i, id, arcs[j].e2, i); id++;
			fprintf(outputFile, "- 9999 x%d.%d.%d ", arcs[j].e1, arcs[j].e2, i);
			fprintf(outputFile, "+ 9999 z%d.%d.%d ", arcs[j].e1, arcs[j].e2, i);
			fprintf(outputFile, ">= 0\n");
			fprintf(outputFile, "c12.%d.%d : s%d.%d ", i, id, arcs[j].e1, i); id++;
			fprintf(outputFile, "- 9999 x%d.%d.%d ", arcs[j].e2, arcs[j].e1, i);
			fprintf(outputFile, "+ 9999 z%d.%d.%d ", arcs[j].e1, arcs[j].e2, i);
			fprintf(outputFile, ">= 0\n");
		}
	}
	fprintf(outputFile, "\n");
}

void shortest_paths_constraints()
{
	int i, j;
	for (i = 1; i <= nb_periods; i++)
	{
		for (j = 0; j < nb_arcs; j++)
		{
			fprintf(outputFile, "c13.%d.%d : s%d.%d ", i, j + 1, arcs[j].e1, i);
			fprintf(outputFile, "- s%d.%d ", arcs[j].e2, i);
			fprintf(outputFile, "+ 9999 x%d.%d.%d ", arcs[j].e1, arcs[j].e2, i);
			fprintf(outputFile, "+ %.3f x%d.%d.%d ", arcs[j].dist, arcs[j].e1, arcs[j].e2, i);
			fprintf(outputFile, "<= 9999\n");
		}
		for (j = 0; j < nb_arcs; j++)
		{
			fprintf(outputFile, "c13.%d.%d : s%d.%d ", i, nb_arcs + j + 1, arcs[j].e2, i);
			fprintf(outputFile, "- s%d.%d ", arcs[j].e1, i);
			fprintf(outputFile, "+ 9999 x%d.%d.%d ", arcs[j].e2, arcs[j].e1, i);
			fprintf(outputFile, "+ %.3f x%d.%d.%d ", arcs[j].dist, arcs[j].e2, arcs[j].e1, i);
			fprintf(outputFile, "<= 9999\n");
		}
	}
	fprintf(outputFile, "\n");
}

void variables_definition ()
{
	int i, t;

	//---------------------
	//bounds
	//---------------------
	fprintf(outputFile, "bounds\n\n");

	for (t = 1; t <= nb_periods; t++)
	{
		for (i = 0; i < nb_nodes; i++)
			if (nodes[i].status > 0 && nodes[i].status < 4)
				fprintf(outputFile, "f0.%d.%d >= 0\n", nodes[i].id, t);
		for (i = 0; i < nb_arcs; i++)
		{
			fprintf(outputFile, "f%d.%d.%d >= 0\n", arcs[i].e1, arcs[i].e2, t);
			fprintf(outputFile, "f%d.%d.%d >= 0\n", arcs[i].e2, arcs[i].e1, t);
		}//end_for
	}
	fprintf(outputFile, "\n");

	for (t = 1; t <= nb_periods; t++)
		for (i = 0; i < nb_nodes; i++)
			fprintf(outputFile, "s%d.%d >= 0\n", nodes[i].id, t);
	fprintf(outputFile, "\n");

	for (t = 1; t <= nb_periods; t++)
	{
		for (i = 0; i < nb_arcs; i++)
		{
			if (arcs[i].periods > 0)
			{
				fprintf(outputFile, "w%d.%d.%d >= 0\n", arcs[i].e1, arcs[i].e2, t);
				fprintf(outputFile, "w%d.%d.%d >= 0\n", arcs[i].e2, arcs[i].e1, t);
			}
		}
	}
	fprintf(outputFile, "\n");

	for (t = 1; t <= nb_periods; t++)
	{
/*		for (i = 0; i < nb_nodes; i++)
		{
			if (nodes[i].status > 0 && nodes[i].status < 4)
			{
				fprintf(outputFile, "y0.%d.%d >= 0\n", nodes[i].id, t);
			}
		}
*/		for (i = 0; i < nb_arcs; i++)
		{
			fprintf(outputFile, "y%d.%d.%d >= 0\n", arcs[i].e1, arcs[i].e2, t);
			fprintf(outputFile, "y%d.%d.%d >= 0\n", arcs[i].e2, arcs[i].e1, t);
		}//end_for
	}
	fprintf(outputFile, "\n\n");

	//---------------------
	//binaries
	//---------------------
	fprintf(outputFile, "binaries\n\n");
	
	for (t = 1; t <= nb_periods; t++)
	{
		for (i = 0; i < nb_arcs; i++)
		{
			fprintf(outputFile, "x%d.%d.%d\n", arcs[i].e1, arcs[i].e2, t);
			fprintf(outputFile, "x%d.%d.%d\n", arcs[i].e2, arcs[i].e1, t);
		}//end_for
	}
	fprintf(outputFile, "\n");

	for (t = 0; t <= nb_periods; t++)
		for (i = 0; i < nb_arcs; i++)
			if (arcs[i].periods > 0)
				fprintf(outputFile, "z%d.%d.%d\n", arcs[i].e1, arcs[i].e2, t);
	fprintf(outputFile, "\n\n");

	//---------------------
	//general
	//---------------------
	fprintf(outputFile, "general\n\n");

	for (t = 1; t <= nb_periods; t++)
	{
		for (i = 0; i < nb_nodes; i++)
		if (nodes[i].status > 0 && nodes[i].status < 4)
			fprintf(outputFile, "f0.%d.%d\n", nodes[i].id, t);
		for (i = 0; i < nb_arcs; i++)
		{
			fprintf(outputFile, "f%d.%d.%d\n", arcs[i].e1, arcs[i].e2, t);
			fprintf(outputFile, "f%d.%d.%d\n", arcs[i].e2, arcs[i].e1, t);
		}//end_for
	}
	fprintf(outputFile, "\n");

	for (t = 1; t <= nb_periods; t++)
	for (i = 0; i < nb_nodes; i++)
		fprintf(outputFile, "s%d.%d\n", nodes[i].id, t);
	fprintf(outputFile, "\n");

	for (t = 1; t <= nb_periods; t++)
	{
		for (i = 0; i < nb_arcs; i++)
		{
			if (arcs[i].periods > 0)
			{
				fprintf(outputFile, "w%d.%d.%d\n", arcs[i].e1, arcs[i].e2, t);
				fprintf(outputFile, "w%d.%d.%d\n", arcs[i].e2, arcs[i].e1, t);
			}
		}//end_for
	}
	fprintf(outputFile, "\n");

	for (t = 1; t <= nb_periods; t++)
	{
/*		for (i = 0; i < nb_nodes; i++)
		{
			if (nodes[i].status > 0 && nodes[i].status < 4)
			{
				fprintf(outputFile, "y0.%d.%d\n", nodes[i].id, t);
			}
		}
*/		for (i = 0; i < nb_arcs; i++)
		{
			fprintf(outputFile, "y%d.%d.%d\n", arcs[i].e1, arcs[i].e2, t);
			fprintf(outputFile, "y%d.%d.%d\n", arcs[i].e2, arcs[i].e1, t);
		}//end_for
	}
	fprintf(outputFile, "\n\n");
	fprintf(outputFile, "end");
}//end_variables_definition