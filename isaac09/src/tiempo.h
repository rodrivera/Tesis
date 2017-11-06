/* Searches using  Horspool's algorithm adapted to 
search inside a text compressed with End-Tagged Dense Code.
Lightweight Natural Language Text Compression: Information Retrieval 2006

Programmed by Antonio Fariña.

Author's contact: Antonio Fariña, Databases Lab, University of
A Coruña. Campus de Elviña s/n. Spain  fari@udc.es

Copyright (C) 2006  Nieves R. Brisaboa, Gonzalo Navarro, Antonio Fariña and José R. Paramá
Author's contact: antonio.fari@gmail.com

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/


/* tiempo.h: Gives support to take time measures 
	Using either getTiempo we take note of the current moment of "time"
	Differences between two moments of time permit to obtain execution time.
*/
#ifndef TIEMPO_H
#define TIEMPO_H
#endif

#include <sys/resource.h>
#include <sys/time.h>
#include <malloc.h>
#include <stdio.h>

	struct TiempoDato{
	  double user;
	  double sys;
	  double total;
	};

	typedef struct TiempoDato tTiempo;

	/* By using gettimeofday and getrusage, it obtains current time. */
//	void getTiempo(tTiempo *tiempo);
	/* Obtains the difference in time between two moments (tTiempo) and displays it.  */
//	void muestraTiempo(tTiempo mitiempo1, tTiempo mitiempo2,char *mensaje);
void getTiempo(tTiempo *tiempo)
{
	double mega = 1.0e-6;
	struct rusage buffer;
	struct timeval tp;
	struct timezone tzp;

	getrusage(RUSAGE_SELF, &buffer);
	gettimeofday(&tp, &tzp);
//	tiempo->user = (double) buffer.ru_utime.tv_sec * 1000000 +
//				     buffer.ru_utime.tv_usec;

	tiempo->user = (double) buffer.ru_utime.tv_sec +
				    1.0e-6 * buffer.ru_utime.tv_usec;
//	tiempo->sys   = (double) buffer.ru_stime.tv_sec +
//				    1.0e-6 * buffer.ru_stime.tv_usec;
	tiempo->total = (double) tp.tv_sec + 1.0e-6 * tp.tv_usec;
}



/*------------------------------------------------------------------
 Obtains the difference in time between two moments (tTiempo) and displays it.
 ------------------------------------------------------------------ */
void muestraTiempo(tTiempo mitiempo1, tTiempo mitiempo2,char *mensaje)
{
	printf("   *******************************************\n");
	printf("   *%s  ",mensaje);
	// printf("   * real  %f\n",  mitiempo2.total - mitiempo1.total );
	printf("   * user  %f\n",  mitiempo2.user - mitiempo1.user );
	// printf("   * sys   %f\n",  mitiempo2.sys - mitiempo1.sys );
	// printf("   * user +sys   %f\n",  mitiempo2.sys - mitiempo1.sys + mitiempo2.user - mitiempo1.user);
	printf("   *******************************************");
	printf("\n ");

}
