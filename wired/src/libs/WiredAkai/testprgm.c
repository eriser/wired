#include "akai.h"

void main()
{
	t_akaiProgram *prgm;
	int i, s;

	char *prgmpath = "/dev/cdrom:1:01 VI TR FF:VI TRMOLO FF";
	prgm = akaiLoadProgram("/dev/cdrom", 2, "VLNS A", "VIOLINS LN");
	if (prgm)
	{
		printf("Loaded %s!\n", prgmpath);
		printf("Name: %s\n", prgm->name);
		printf("Num: %i\n", prgm->num);
		printf("Channel: %i\n", prgm->channel);
		printf("hikey: %i\n", prgm->highkey);
		printf("lokey: %i\n", prgm->lowkey);
		printf("nbkeygrp: %i\n", prgm->nkeygrps);
		for (i = 0; prgm->keygrp; prgm->keygrp = prgm->keygrp->next)
		{
			t_akaiKeygrp *k = elt(prgm->keygrp, t_akaiKeygrp *);
			printf ("Keygroup %i : \n", i++);
			printf ("num : %i\n", k->num);
			printf ("hikey: %i\n", k->highkey);
			printf ("lokey: %i\n", k->lowkey);
			printf ("tune: %i\n", k->tune);
			printf ("tune_semi: %i\n", k->tune_semi);
			printf ("env A : %i %i\n", k->env_A[0], k->env_A[1]);
			printf ("env D : %i %i\n", k->env_D[0], k->env_D[1]);
			printf ("env S : %i %i\n", k->env_S[0], k->env_S[1]);
			printf ("env R : %i %i\n", k->env_R[0], k->env_R[1]);
			printf ("env A rate : %i %i\n", k->env_A_rate[0], k->env_A_rate[1]);
			printf ("env D rate : %i %i\n", k->env_D_rate[0], k->env_D_rate[1]);
			printf ("env S rate : %i %i\n", k->env_S_rate[0], k->env_S_rate[1]);
			printf ("env R rate : %i %i\n", k->env_R_rate[0], k->env_R_rate[1]);
			printf ("zone: %s / %s / %s / %s\n", k->zone[0], k->zone[1], k->zone[2], k->zone[3]);
			for (s = 0; s < 4; s++)
			{
				if (!k->zone_sample[s])
					printf("Sample %i is NULL\n", s);
				else
				{
					t_akaiSample *sp = k->zone_sample[s];
					printf("Sample %i : \n", s);
					printf("  Name : %s\n", sp->name);
					printf("  Tune : %u\n", sp->tune);
					printf("  Size : %u\n", sp->size);
					printf("  start : %u\n", sp->start);
					printf("  end : %u\n", sp->end);
					printf("  loop_start: %u\n", sp->loop_start);
					printf("  loop_len: %u\n", sp->loop_len);
					printf("  loop_times: %u\n", sp->loop_times);
					printf("  rate: %u\n", sp->rate);
					printf("  basenote: %u\n", sp->base_note);
					printf("  channels : %u\n", sp->channels);
					printf("  buffer: %p\n", sp->buffer);
					printf("  -------------------------------------\n");
				}
			}
			printf ("zone hi vel: %i %i %i %i\n", k->zone_high_vel[0], k->zone_high_vel[1], k->zone_high_vel[2], k->zone_high_vel[3]); 
			printf ("zone lo vel: %i %i %i %i\n", k->zone_low_vel[0], k->zone_low_vel[1], k->zone_low_vel[2], k->zone_low_vel[3]); 
			printf ("zone pan: %i %i %i %i\n", k->zone_pan[0], k->zone_pan[1], k->zone_pan[2], k->zone_pan[3]); 
			printf ("-----------------------------\n");
		}
	}
}
