
#pragma inline-stdfuncs (on)
#pragma static-locals (on)
#pragma register-vars (on)

#define NDEBUG  // NO DEBUG - release/fast build

// STANDARD/CC65 LIBRARY
#include <conio.h>           // gotoxy, cprintf   NOTE: this brings in stdargs for variable arguments support
#include <stdio.h>           // printf
#include <stdlib.h>          // malloc, free, srand, rand,  EXIT_SUCCESS

#include <c_vector.h>         // vec_new, vec_delete, vec_push_back, vec_get

// LOCAL SUPPORT LIBRARY
#include <utility.h>
#include <destiny_structs.h>

#define MAX_SYMBOL_PER_RLE 32   // limited to this number+1  (32) due to only have 5 bits of storage
void decode_screen_PEEK(unsigned char* temp_symbol, unsigned char* block_toggle)
{
	if (
		(*temp_symbol == 32)
		|| (*temp_symbol == 160)
	)
	{
		  *block_toggle = FALSE;
		  *temp_symbol = ' ';
	}
	else
	{
		  if (*temp_symbol < 91)
		  {
			  // lower case, convert back to upper case
			  *temp_symbol += 128;
			  *block_toggle = cpeekrevers();
		  }
		  else
		  {
			  if (IS_BIT_ON(*temp_symbol, BYTE_TOP) == TRUE)
			  {
				  *block_toggle = TRUE;
				  CLEAR_BIT(*temp_symbol, BYTE_TOP);
				  *temp_symbol += 128;
			  }
			  else
			  {
				  *block_toggle = FALSE;
			  }					  
		  }			  
	}
}
#define MAX_BLOCKER_DATA_ROWS 23  // note index 0 and 1 are wasted, but keep Y 1:1 with screen, just like X (avoid have to +2/-2 all the time)
#define MAX_BLOCKER_DATA_COL 5
unsigned char rle_encode_map(unsigned char lvl_index)
{
	unsigned char current_symbol;
	unsigned char new_symbol;
	unsigned char block_toggle = FALSE;
	unsigned char len;
	unsigned char new_x;
	unsigned char new_y;	
	unsigned int total = 0;
	unsigned int current_symbol_count = 0;
	unsigned char result = TRUE;
	FILE* fout = 0;
	char buffer[40];
	Blocker temp_blocker;
	RLE temp_rle;
	
	Blocker* ptr_blocker;
	RLE *ptr_rle;

	static vec* pvec_rle;
	static vec* pvec_blockers;
	
	static char blocker_bits[MAX_BLOCKER_DATA_ROWS][MAX_BLOCKER_DATA_COL];  // ROW, COL-portion   (5-bits covers 40 columns, just need 39)

	pvec_rle = vec_new_with_reserve(sizeof(RLE), MAX_RLE_ENTRIES); //sizeof(RLE), 100);	
	pvec_blockers = vec_new_with_reserve(sizeof(Blocker), MAX_BLOCKER_ENTRIES); //sizeof(RLE), 100);	
		
  memset(blocker_bits, 0, sizeof(blocker_bits));  // initially set all blocker bits to 0 (including the first two rows that remain untouched)

	current_symbol = '\0';  // current RLE not yet initialized (don't know the first tile symbol)
	g_i = 0;
		
	for (new_y = 2; new_y < 23; ++new_y)
	{
		for (new_x = 0; new_x < MAX_BOARD_WIDTH-1; ++new_x)  //-1 since we are ignoring the 40th column due to "newline" issues
		{
			block_toggle = FALSE;
			
			new_symbol = PEEK(0x8000 + (MAX_BOARD_WIDTH * new_y) + new_x);  // 'a' - 'z'  65 to 90
			decode_screen_PEEK(&new_symbol, &block_toggle);
			
			if (block_toggle)
			{			
				if (len == MAX_BLOCKER_ENTRIES)
				{
					// already at max and we wanted to add another!? no way...					
					DO_BEEP;
					
					vec_delete(pvec_rle);
					vec_delete(pvec_blockers);						
					return FALSE;
				}
				
				SET_BIT( 
				  blocker_bits[new_y][(new_x / 8)],					
				  (new_x % 8)   // "8 - x" is reverse order so we can match x columns
				);
				
				temp_blocker.x = new_x;
				temp_blocker.y = new_y;
				vec_push_back(pvec_blockers, &temp_blocker);
				
				gotoxy(35,1);   //< caution: this seems necessary to get LEN initilized correctly - very voodoo
				len = VEC_LENGTH(pvec_blockers);				
				cprintf("B%3u", len);								
			}			
			
			if (new_symbol == current_symbol)
			{
				++current_symbol_count;
			}
			
			if (
			  ( (new_symbol != current_symbol) )
			  || ( current_symbol_count == MAX_SYMBOL_PER_RLE )
			  || ( (new_y == 22) && (new_x == (MAX_BOARD_WIDTH-1)-1) )
			)				
			{
				if (current_symbol == '\0')
				{
					current_symbol = new_symbol;
					current_symbol_count = 1;
				}
				else  // transitioning to a new symbol, store down count of prior symbol...
				{
					if (current_symbol_count > 0)
					{
						// STORE RLE <current_symbol><current_symbol_count>
						
						switch (current_symbol)
						{
						case 'W':
						case 'w': temp_rle.symbol = 0; break;
						case 'B':
						case 'b': temp_rle.symbol = 1; break;
						case 'G':
						case 'g': temp_rle.symbol = 2; break;
						case 'R':
						case 'r': temp_rle.symbol = 3; break;
						case 'S':
						case 's': temp_rle.symbol = 4; break;
						case 'Z':
						case 'z': temp_rle.symbol = 5; break;					
						default:
						  {
							DO_BEEP;
							exit(-5);
						  }
						  break;
						}						
						
						temp_rle.length = current_symbol_count - 1;  // "1" is implied by existence of this RLE entry
						temp_rle.encoded = (temp_rle.symbol << 5);
						temp_rle.encoded |= temp_rle.length;
						
						vec_push_back(pvec_rle, &temp_rle);
			
						if ( current_symbol_count == 32 )			
						{
							current_symbol = '\0';
							current_symbol_count = 0;
						}
						else
						{
							current_symbol = new_symbol;
							current_symbol_count = 1;
						}
					}
					else
					{
						DO_BEEP;
					}
				}
				
			}			
		}
	}
	
	/*
	clrscr();
	total = 0;	
	new_y = VEC_LENGTH(pvec_rle_raw);
	printf("rle_raw len %u\n", new_y);
	for (g_i = 0; g_i < new_y; ++g_i)
	{
		ptr_rle_raw = VEC_GET(pvec_rle_raw, g_i);
		printf("%2u %u %3u | ", g_i, ptr_rle_raw->symbol, ptr_rle_raw->count);
		
		total += ptr_rle_raw->count;
		
		if (g_i % 3 == 0)
		{
			printf("\n");
		}
	}
	printf("%u\n", total);
	len = VEC_LENGTH(pvec_blockers);
	printf("blocker count = %u\n", len);
	exit(-5);
	*/
		
	if (total != 819)
	{
		gotoxy(0,23);
		printf("ERR[%u]", total);		
	}
	
	gotoxy(0, 23);
	cprintf("WRITING RLE+BLK");
	
	/*
	clrscr();
	
	new_x = 0;
	total = 0;
	len = VEC_LENGTH(pvec_rle);	
	for (g_i = 0; g_i < len; ++g_i)
	{
		if (((g_i+1) % 22) == 0)
		{
			new_x += 11;
		}
		
		ptr_rle = VEC_GET(pvec_rle, g_i);
		total += ptr_rle->length;
		
		if (new_x < 30)
		{
		  // "12 5 31 123|"
		  gotoxy(new_x, (g_i+1) % 22);
		  cprintf("%2u %u %2u %2x|", (g_i+1), ptr_rle->symbol, ptr_rle->length, ptr_rle->encoded);
		}
	}
	gotoxy(0, 23);
	cprintf("[%u] RLE entries [total %u --> %u]", len, total, total + len);
	*/
		
	sprintf(buffer, "rle_%u_b.txt", lvl_index);
	fout = fopen(buffer, "w");
	if (fout != 0)
	{		
		len = VEC_LENGTH(pvec_rle);	
		for (g_i = 0; g_i < len; ++g_i)
		{			
			ptr_rle = VEC_GET(pvec_rle, g_i);			
			//new_x = sprintf(buffer, "  \\x%2X,  // %2u %u\n");
			new_x = sprintf(buffer, "%u %u %u\n", ptr_rle->encoded, ptr_rle->symbol, ptr_rle->length);
			
			fwrite(buffer, new_x, 1, fout);
		}
		
		new_x = sprintf(buffer, "255 255 %u\n", len);
		fwrite(buffer, new_x, 1, fout);
		
		fclose(fout);	
		
		gotoxy(0, 24);
	  cprintf("RLE OK!");
	}
	else
	{
		gotoxy(0, 24);
	  cprintf("RLE BAD");
	}

  sprintf(buffer, "blockers_%u.txt", lvl_index);
  fout = fopen(buffer, "w");
	if (fout != 0)
	{
		len = VEC_LENGTH(pvec_blockers);
		for (g_i = 0; g_i < len; ++g_i)
		{			
			ptr_blocker = VEC_GET(pvec_blockers, g_i);
			//new_x = sprintf(buffer, "  x = %3u; y = %3u;\n", ptr_blocker->x, ptr_blocker->y);
			new_x = sprintf(buffer, "%u %u\n", ptr_blocker->x, ptr_blocker->y);
			
			fwrite(buffer, new_x, 1, fout);
		}
		new_x = sprintf(buffer, "255 255\n");
		fwrite(buffer, new_x, 1, fout);
		fwrite(buffer, new_x, 1, fout);

        for (new_y = 0; new_y < MAX_BLOCKER_DATA_ROWS; ++new_y)
		{
			for (new_x = 0; new_x < MAX_BLOCKER_DATA_COL; ++new_x)
			{
				new_x = sprintf(buffer, "%u [%u,%u,%u,%u,%u],\n",  // can not use %x here - won't be compatible when transfer off PET
				  new_y, 
				  blocker_bits[new_y][new_x],
				  blocker_bits[new_y][new_x+1],
				  blocker_bits[new_y][new_x+2],
				  blocker_bits[new_y][new_x+3],
				  blocker_bits[new_y][new_x+4]
				);			
			    fwrite(buffer, new_x, 1, fout);
			}
		}
		
		fclose(fout);	
		
		gotoxy(8, 24);
	    cprintf("BLK OK!");
	}
	else
	{
		gotoxy(0, 24);
	    cprintf("BLK BAD");
	}
	
	vec_delete(pvec_rle);
	vec_delete(pvec_blockers);						
	
	gotoxy(0, 23);
	cprintf("WRITING DONE   ");
	
	return result;
}

unsigned char load_map(unsigned char lvl_index, unsigned char curr_x, unsigned char curr_y, unsigned char* is_reverse)
{
	char buffer[40];
	FILE* fin = 0;
	char* sf_result;
	unsigned char i;
	unsigned char x = 0; 
	unsigned char y = 2;
	unsigned char temp1;
	unsigned char temp2;
	unsigned int total = 0;
	unsigned char display_symbol;
	unsigned char ch_result;
	RLE temp_rle;
	Blocker temp_blocker;
	Blocker* ptr_blocker;
	vec* pvec_blockers;
	
	*is_reverse = FALSE;

	pvec_blockers = vec_new_with_reserve(sizeof(Blocker), MAX_BLOCKER_ENTRIES); //sizeof(RLE), 100);	

	sprintf(buffer, "blockers_%u.txt", lvl_index);
	fin = fopen(buffer, "r");
	if (fin != 0)
	{
		//new_x = sprintf(buffer, "%3u %3u\n", ptr_blocker->x, ptr_blocker->y);
		while (TRUE)
		{
			sf_result = fgets(buffer, sizeof(buffer), fin);
			if (
			  (feof(fin) == TRUE)			  
			  || (sf_result == NULL)
			)
			{
				break;
			}
			
			//new_x = sprintf(buffer, "%2X\n", ptr_rle->encoded);
			
			sscanf(buffer, "%u %u", &temp_blocker.x, &temp_blocker.y);
			if (
			  (
			    (temp_blocker.x == 0)
			    && (temp_blocker.y == 0)
			  )
			||
			  (
			    (temp_blocker.x == 255)
			    && (temp_blocker.y == 255)
		      )			  
			)
			{
				// end of data marker - done reading for purposes of the editor
				break;
			}
			vec_push_back(pvec_blockers, &temp_blocker);
		}
		
		fclose(fin);
	}		
	
	sprintf(buffer, "rle_%u_b.txt", lvl_index);
	
	/*
	fin = fopen(buffer, "r");	
	if (fin != 0)
	{
		clrscr();
		
		while (TRUE)
		{
			result = fgets(buffer, sizeof(buffer), fin);
			if (
			  (feof(fin) == TRUE)			  
			  || (result == NULL)
			)
			{
				break;
			}
			
			sscanf(buffer, "%u", &temp_rle.encoded);			
										
			temp_rle.symbol = ((temp_rle.encoded & 0xE0) >> 5) & 0x07;
			temp_rle.length = (temp_rle.encoded & 0x1F) + 1;			
			++display_symbol;
			total += temp_rle.length;
			
			printf("%2u %u %2u %u\n", display_symbol, temp_rle.symbol, temp_rle.length, total);	
		}
		fclose(fin);
	}
	total = 0;
	g_enter_result = wait_for_ENTER();
	*/
		
	fin = fopen(buffer, "r");
	
	if (fin != 0)
	{
		while (TRUE)
		{
			sf_result = fgets(buffer, sizeof(buffer), fin);
			if (
			  (feof(fin) == TRUE)			  
			  || (sf_result == NULL)
			)
			{
				break;
			}
			
			//new_x = sprintf(buffer, "%2X\n", ptr_rle->encoded);
			
			sscanf(buffer, "%u %u %u", &temp_rle.encoded, &temp1, &temp2);
			
			if (
			  (temp_rle.encoded == 255)
			  || (temp1 == 255)
			)
			{
				break;
			}
																						
            // if these computed symbol and length don't match the ENCODED... then something is amiss
			temp_rle.symbol = ((temp_rle.encoded & 0xE0) >> 5) & 0x07;  // 0x07 needed to avoid taking the sign bit?
			temp_rle.length = (temp_rle.encoded & 0x1F) + 1;
					
			gotoxy(0, 24);
			cprintf("%u %2u|", temp_rle.symbol, temp_rle.length);					
			
			if (temp1 != temp_rle.symbol)
			{
				DO_BEEP;
				temp1 = wait_for_ENTER();
			}
			if ((temp2+1) != temp_rle.length)
			{
				DO_BEEP;
				temp1 = wait_for_ENTER();
			}			
			
			switch (temp_rle.symbol)
			{
			case 0: display_symbol = 'W'; break;
			case 1: display_symbol = 'B'; break;
			case 2: display_symbol = 'G'; break;
			case 3: display_symbol = 'R'; break;
			case 4:	display_symbol = 'S'; break;
			case 5: display_symbol = 'Z'; break;
			default:
			  gotoxy(20,24);
			  printf(">%u %2x<", display_symbol, temp_rle.encoded);
			  DO_BEEP;
			  exit(-5);
			  break;					
			}
			
			gotoxy(x,y);
			while (temp_rle.length > 0)
			{			
				if (
				  (x == curr_x) && (y == curr_y)
				)
				{
					ch_result = display_symbol;
				}		
				//printf("%c", display_symbol);
				
				// Examine the list of blockers for this STAGE... If we're on a blocker cell, then
				// show it in REVERSE
				for (i = 0; i < VEC_LENGTH(pvec_blockers); ++i)
				{
					ptr_blocker = VEC_GET(pvec_blockers, i);
					if (
					  (ptr_blocker->x == x)
					  && (ptr_blocker->y == y)
					)
					{
						ENABLE_REVERSE_MODE;
						printf("%c", display_symbol);
						ENABLE_REGULAR_MODE;
						
						if ((x == curr_x) && (y == curr_y))
						{
							(*is_reverse) = TRUE;  // indicate that the cell we're located on when we loaded the map was in REVERSE mode
						}
						break;
					}
				}
				if (i == VEC_LENGTH(pvec_blockers))
				{				
			      // not a blocker, show regular...
				  cputc(display_symbol);
				}
				
				++total;
				if (total == 819)
				{
					break;  
				}	
				
				++x;
				if (x == 39)
				{
					x = 0;
					++y;
					gotoxy(x,y);
				}								
				--temp_rle.length;
			}
		}
		
		fclose(fin);	
	}
	
	vec_delete(pvec_blockers);
		
	return ch_result;
}

void run_map_editor()
{
    // A == 193
	// a == 65
	// delta 128

    // Going to use the screen buffer as the memory for map settings $8000
	
	unsigned char current_x = 0;
	unsigned char current_y = 2;
	unsigned char new_x;
	unsigned char new_y;
	unsigned char block_toggle = FALSE;  // TRUE means reverse mode is ON/ENABLED
	unsigned char cursor_blink = FALSE;
	unsigned char current_symbol = ' ';	
	unsigned char lvl_index = 1;
	
	Time_counter now_timer;
		
//	ENABLE_GRAPHIC_MODE;
	
	clrscr();
	
	/*
	// Little test program to verify values of A and a, with and without reverse...
	//ENABLE_REVERSE_MODE;
	global_input_ch = ' ';
	printf("%c=%3u", global_input_ch, global_input_ch);
	//ENABLE_REGULAR_MODE;
	global_input_ch = ' ';
	printf("%c=%3u", global_input_ch, global_input_ch);
	
	gotox(0);
	current_x = cpeekrevers();
	current_y = PEEK(0x8000);
	printf("\nrev=%u", current_x);
	printf("\npeek=%u", current_y);	
	exit(-5);		
	*/
	
	// PEEK results....
	// GRAPH_MODE
	// char 1 = 193(spade),A=65
	
	// TEXT_MODE
	// char 1 = a (lowercase)     129 reverse a   (bit 8 is ON, then 1 to 26)
	// char 2 = b                 130         b
	//                            131 reverse  c
	// 							  132 reverse  d
	//                            133 reverse  e
	//                            134 reverse  f 
	//                            135 reverse  g
	//                            136 reverse  h 
	//                            137 reverse  i
	//                            138 reverse  j 
	//                            139 reverse  k 
	//                            140 reverse  l
	//                            141 reverse  m
	//                            142 reverse  n 
	//                            143 reverse  o 
	//                            144 reverse  p
	//                            145 reverse  q 
	//                            146 reverse  r 
	//                            147 reverse  s
	//                            148 reverse  t
	//                            149 reverse  u 
	//                            150 reverse  v
	//                            151 reverse  w
	//                            152 reverse  x 
	//                            153 reverse  y
	// char 26 = z (lowercase)    154 reverse z		
		
	     // 1234567890123456789012345678901234567890
	gotoxy(0,0);
	cprintf("arrws move|space=toggle blck|+- idx|");
	gotoxy(0,1);
	cprintf("W=wtr,B=bch,G=grs,R=rck,S=spa,Z=spl");
	
	gotoxy(0,23);
	cprintf("Q=fill|E=to end|X=RLE compress|L=load");
	
		     
	gotoxy(0,2);
         //  1234567890123456789012345678901234567890	
	cprintf("WWWBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB ");
	cprintf("WWWWBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB ");
	cprintf("WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW ");
	cprintf("WWWWWWGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG ");
	cprintf("WWWWWWWGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG ");
	cprintf("WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW ");
	cprintf("WWWWWWWWWRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR ");
	cprintf("WWWWWWWWWWRRRRRRRRRRRRRRRRRRRRRRRRRRRRR ");
	cprintf("WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW ");
	cprintf("WWWWWWWWWWWWSSSSSSSSSSSSSSSSSSSSSSSSSSS ");
	cprintf("WWWWWWWWWWWWWSSSSSSSSSSSSSSSSSSSSSSSSSS ");
	cprintf("WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW ");
	cprintf("WWWWWWWWWWWWWWWZZZZZZZZZZZZZZZZZZZZZZZZ ");
	cprintf("WWWWWWWWWWWWWWWWZZZZZZZZZZZZZZZZZZZZZZZ ");
	cprintf("WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW ");
	cprintf("WWWWWWWWWWWWWWWWWBBBBBBBBBBBBBBBBBBBBBB ");
	cprintf("WWWWWWWWWWWWWWWWWGGGGGGGGGGGGGGGGGGGGGG ");
	cprintf("WWWWWWWWWWWWWWWWWRRRRRRRRRRRRRRRRRRRRRR ");
	cprintf("WWWWWWWWWWWWWWWWWSSSSSSSSSSSSSSSSSSSSSS ");
	cprintf("WWWWWWWWWWWWWWWWWZZZZZZZZZZZZZZZZZZZZZZ ");
	cprintf("WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW ");
    goto go_do_over;
	
full_refresh_board:	
    gotoxy(0,2);

    // NOTE this seems to clear hidden blocker settings
	if (current_symbol != ' ')
	{
		for (new_y = 2; new_y < 23; ++new_y)
		{
		  gotoxy(0, new_y);
		  for (new_x = 0; new_x < 39; ++new_x)
  		  {
			
				//gotoxy(new_x, new_y);
				//printf("%c", current_symbol);  
				cputc(current_symbol);
			}
		}
	}
	
go_do_over:		
    new_x = current_x;
	new_y = current_y;	
	
	STORE_TIME(global_timer);
	
    while (TRUE)
	{
		gotoxy(38,0);
		cprintf("%u", lvl_index);
		
		STORE_TIME(now_timer);
		
		gotoxy(20, 24);
		cprintf("X%3u Y%3u", current_x, current_y);
		
		gotoxy(30, 24);
		if (block_toggle == TRUE)
		{			
			printf("BLK ON ");
		}
		else
		{
			printf("BLK OFF");
		}
		
		gotoxy(current_x, current_y);		
		if (current_symbol != ' ')
		{
		  if (block_toggle == TRUE) 
		  {
			  ENABLE_REVERSE_MODE;
		      printf("%c", current_symbol);
			  ENABLE_REGULAR_MODE;
		  }
		  else
		  {
			  cputc(current_symbol);
		  }
		}
		else
		{
			cputc(' ');  // erase * just in case
		}
		gotoxy(current_x, current_y);
		
		global_input_ch = kbhit();
		if (global_input_ch != 0)  // if a key was pressed...
		{	
			global_input_ch = cgetc();
			
			if (
			  (global_input_ch == 'X')
			  || (global_input_ch == 'x')
			)
			{
				// done creating, new output compute and output the compressed format
				break;
			}
			
			switch (global_input_ch)
			{
			case 'W':
			case 'w': current_symbol = 'W'; break;
			case 'B':
			case 'b': current_symbol = 'B'; break;
			case 'G':
			case 'g': current_symbol = 'G'; break;
			case 'R':
			case 'r': current_symbol = 'R'; break;
			case 'S':
			case 's': current_symbol = 'S'; break;
			case 'Z':
			case 'z': current_symbol = 'Z'; break;						
			
			case ']':
			case '>':
			case '+': if (lvl_index < 8) ++lvl_index; break;
			
			case '[':
			case '<':
			case '-': if (lvl_index > 1) --lvl_index; break;			  
			
			case 'L':
			case 'l': 
			  {
				//new_x = 0;
				//new_y = 2;				
			    current_symbol = load_map(lvl_index, current_x, current_y, &block_toggle);
				break;
			  }

			case 32:  // SPACEBAR
			  block_toggle = !block_toggle;
			  break;					
			
			case KEY_UP_ARROW:			  
			  if (current_y > 2) 
				  new_y = current_y-1;
			  else
				  new_y = 22;
			  break;
			  
			case KEY_DOWN_ARROW:
			  if (current_y < 22)			  
				  new_y = current_y+1;			  
			  else
				  new_y = 2;
			  break;
			  
			case KEY_LEFT_ARROW:
			  if (current_x > 0) 
				  new_x = current_x-1;
			  else
				  new_x = 38;
			  break;
			  
			case KEY_RIGHT_ARROW:
			  if (current_x < 38) 
				  new_x = current_x+1;
			  else
				  new_x = 0;
			  break;
			  
			case 'Q':
			case 'q': 
			  {
				goto full_refresh_board; 
				break;
			  }
			  
			case 'E':
			case 'e':
			  {
				  g_i = 39 - wherex();
				  while (g_i > 0)
				  {
					  cputc(current_symbol);					  
					  --g_i;
				  }				  
			  }
			  break;
			  
			default:			  
			  break;			
			}
			
			if (
			  (current_x != new_x)
			  || (current_y != new_y)
			)
			{
			  current_x = new_x;
			  current_y = new_y;
			  gotoxy(current_x, current_y);  // whether moved or not, get the symbol at the current location
			  current_symbol = PEEK(0x8000 + (40 * current_y) + current_x);  // 'a' - 'z'  65 to 90
			  decode_screen_PEEK(&current_symbol, &block_toggle);
			}
			
		}
		else
		{			
			  UPDATE_DELTA_JIFFY_ONLY(now_timer, global_timer);
			  if (delta_time > 20)
			  {
				  cursor_blink = !cursor_blink;
				  STORE_TIME(global_timer);
			  }
			  if (cursor_blink == TRUE)
			  {
				  gotoxy(current_x, current_y);		
				  cputc(' ');
			  }
		}		
	}
	
	g_i = rle_encode_map(lvl_index);
	goto go_do_over;
	/*
	if (g_i == FALSE)
	{
		goto go_do_over;
	}
	*/
}

int main(void)
{		
/*
    unsigned char new_x;
    unsigned char new_y;
	char buffer[40];
	
	unsigned char temp_x;
	unsigned char temp_y;

    char blocker_bits[MAX_BLOCKER_DATA_ROWS][MAX_BLOCKER_DATA_COL];  // ROW, COL-portion   (5-bits covers 40 columns, just need 39)
    memset(blocker_bits, 0, sizeof(blocker_bits));  // initially set all blocker bits to 0


	for (new_y = 2; new_y < 23; ++new_y)
	{
		for (new_x = 0; new_x < MAX_BOARD_WIDTH-1; ++new_x)  //-1 since we are ignoring the 40th column due to "newline" issues
		{
			  if (
			  (new_y == 19)
			  && (new_x > 32)
			  )
			  {
				  temp_x = (new_x / 8);
				  temp_y = 8 - (new_x % 8);
				  printf("byte_ofs [%u]  bit_ofs [%u]\n", temp_x, temp_y);
				  
                SET_BIT( 
				  blocker_bits[new_y][(new_x / 8)],
				  8 - (new_x % 8)   // "8 - x" is reverse order so we can match x columns
				);
			  }   
		}
	}
	
	for (new_y = 10; new_y < MAX_BLOCKER_DATA_ROWS; ++new_y)
	{
		for (new_x = 0; new_x < MAX_BLOCKER_DATA_COL; ++new_x)
		{
			new_x = sprintf(buffer, "%u [%2X,%2X,%2X,%2X,%2X],\n", 
			  new_y, 
			  blocker_bits[new_y][new_x],
			  blocker_bits[new_y][new_x+1],
			  blocker_bits[new_y][new_x+2],
			  blocker_bits[new_y][new_x+3],
			  blocker_bits[new_y][new_x+4]
			);			
		    
			printf("%s", buffer);
		}
	}	
	while (TRUE)
	{
	}
	exit(-2);
	*/

/*
CLEAR_BIT( 
				  blocker_bits[new_y][(new_x / 8)],
				  8 - (new_x % 8)   // "8 - x" is reverse order so we can match x columns
				);
				*/




	INIT_TIMER(global_timer);	
	
    ENABLE_TEXT_MODE;	
	run_map_editor();

  return EXIT_SUCCESS;
}