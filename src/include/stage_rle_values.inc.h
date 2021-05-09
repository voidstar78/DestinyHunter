/*

DATA ARRANGEMENT NOTES

rle_stageX_values

  8-bits; limited by MAX_RLE_ENTRIES  (default/presently 100, try to keep to minimum to save memory)
  
  _ _ _ | _ _ _ _ _ _
  
  Top 3 bits are the MAP STYLE
  Bottom 5 bits are the LENGTH of that MAP STYLE
  
  <STYLE><LENGTH>  derived from RLE Run Length Encoding
  
  Because the <LENGTH> is limited to 5-bits, this does mean a max-range extent of 31 characters.
  However, the presence of a <STYLE><LENGTH> pair implicitly means there is at least 1 copy of the tile.
  Therefore the actual physical length wll be <LENGTH>+1, meaning each pair can represent up to 32-characters.
  
  Some observations:
  
  - If a section of tile spans more than 32 characters, it must be split into multiple RLE pairs.  For example,
    if an entire ROW contains WATER, that's 39 characters.  So it must be W<31> followed by another W<6>,
	noting that 31+1 and 6+1 = 32+7 = 39 (due to the implicit +1 length).
	
  - An unfortunate sequence might end up being 33 characters, which ends up with  "W<31> W<0>" (as an example with water).
    It's unfortunate in that now 2-bytes is needed to represent something that really only needed 1-byte.  This is just
	a side effect of the process and just a curious observation, not a real negative.  Even in the worse case of someone
	deliberating alternating tiles every 33 characters, the end compression is still far greater.
	
  - If we only had 4 tile types, we could have used 2-bits for the <STYLE> instead of 3.  Originally I did that, but
    I kept the extra 3rd bit as a reserve for growth.  So that's a lesson: you don't want to constantly engineer more
	than you need, but you may want to provision for some prudent growth/expansion when defining data structures
	(otherwise it may mean a painful data-conversion experience in the future).   
	
  - Anyhow, if we had an extra bit to then use for <LENGTH>, we'd be able to represent 65-length segments.  In hindsight,
    maybe it would have been better to just use full 2-byte encoding.  This would allow 256-map types (way more then needed
	currently), but allow a full 256 lengths to be represented.  This would allow a full map of WATER to be represented in
	roughly 6 bytes instead of 25 bytes.  This just goes to show the balance/decisions made when doing a compression
	algorithm, there is no singular correct answer (in this case it just depends on how "alternating tile" the map is).

*/

  /*
  
  Original STAGE1 code for historical purposes.  We now "compress" this map data.
  So the first row below would be "W<3> B<22> L<14>" followed by "W<3> B<22> ..." etc.
  i.e. the first two rows is represented by 6-bytes instead of 39*2 bytes.
  
                   //0123456789012345678901234567890123456789    
  strcpy(val_map, "WWWWBBBBBBBBBBBBBBBBBBBBBBLLLLLLLLLLLLLL");  vec_push_back(pvec_map, &val_map);
	strcpy(val_map, "WWWBBBBBBBBBBBBBBBBBBBBBBLLLLLLLLLLLLLLL");  vec_push_back(pvec_map, &val_map);
	strcpy(val_map, "WWBBBBBBBBBBBBBBBBBBBBBBBLLLLLLLLLLLLLLL");  vec_push_back(pvec_map, &val_map);
	strcpy(val_map, "WWBBBBBBBBBBBBBBBBBBBBBBBLLLLLLLLLLLLLLL");  vec_push_back(pvec_map, &val_map);
	strcpy(val_map, "WWBBBBBBBBBBBBBBBBBBBBBBLLLLLLLLLLLLLLLL");  vec_push_back(pvec_map, &val_map);
	strcpy(val_map, "WWWBBBBBBBBBBBBBBBBBBBBBBLLLLLLLLLLLLLLL");  vec_push_back(pvec_map, &val_map);
	strcpy(val_map, "WWWBBBBBBBBBBBBBBBBBBBBBBBBLLLLLLLLLLLLL");  vec_push_back(pvec_map, &val_map);
	strcpy(val_map, "WWWWBBBBBBBBBBBBBBBBBBBBBBBLLLLLLLLLLLLL");  vec_push_back(pvec_map, &val_map);
	strcpy(val_map, "WWWWBBBBBBBBBBBBBBBBBBBBBBBBLLLLLLLLLLLL");  vec_push_back(pvec_map, &val_map);
	strcpy(val_map, "WWWWWBBBBBBBBBBBBBBBBBBBBBBBBLLLLLLLLLLL");  vec_push_back(pvec_map, &val_map);
	strcpy(val_map, "WWWWWBBBBBBBBBBBBBBBBBBBBBBBBLLLLLLLLLLL");  vec_push_back(pvec_map, &val_map);
	strcpy(val_map, "WWWWWBBBBBBBBBBBBBBBBBBBBBBBLLLLLLLLLLLL");  vec_push_back(pvec_map, &val_map);
	strcpy(val_map, "WWWWBBBBBBBBBBBBBBBBBBBBBBBBLLLLLLLLLLLL");  vec_push_back(pvec_map, &val_map);
	strcpy(val_map, "WWWWBBBBBBBBBBBBBBBBBBBBBBBLLLLLLLLLLLLL");  vec_push_back(pvec_map, &val_map);
	strcpy(val_map, "WWWWBBBBBBBBBBBBBBBBBBBBBBBBLLLLLLLLLLLL");  vec_push_back(pvec_map, &val_map);
	strcpy(val_map, "WWWWBBBBBBBBBBBBBBBBBBBBBBBLLLLLLLLLLLLL");  vec_push_back(pvec_map, &val_map);
	strcpy(val_map, "WWWWBBBBBBBBBBBBBBBBBBBBBBLLLLLLLLLLLLLL");  vec_push_back(pvec_map, &val_map);
	strcpy(val_map, "WWWBBBBBBBBBBBBBBBBBBBBBBBBBLLLLLLLLLLLL");  vec_push_back(pvec_map, &val_map);
	strcpy(val_map, "WWWWBBBBBBBBBBBBBBBBBBBBBBBBLLLLLLLLLLLL");  vec_push_back(pvec_map, &val_map);
	strcpy(val_map, "WWWBBBBBBBBBBBBBBBBBBBBBBBBLLLLLLLLLLLLL");  vec_push_back(pvec_map, &val_map);
	strcpy(val_map, "WWBBBBBBBBBBBBBBBBBBBBBBBBLLLLLLLLLLLLLL");  vec_push_back(pvec_map, &val_map);
	*/

static unsigned char rle_stage1_values[] = {
	3
,	42
,	128
,	97
,	128
,	38
,	76
,	2
,	43
,	131
,	37
,	77
,	1
,	54
,	77
,	1
,	54
,	77
,	2
,	52
,	78
,	2
,	53
,	77
,	3
,	54
,	131
,	71
,	3
,	52
,	130
,	97
,	128
,	71
,	4
,	51
,	128
,	99
,	129
,	70
,	4
,	50
,	129
,	100
,	128
,	70
,	4
,	50
,	128
,	98
,	32
,	97
,	128
,	70
,	3
,	51
,	128
,	101
,	128
,	70
,	3
,	51
,	129
,	98
,	130
,	70
,	3
,	52
,	129
,	98
,	128
,	71
,	3
,	53
,	132
,	71
,	3
,	130
,	51
,	75
,	2
,	98
,	128
,	50
,	76
,	1
,	99
,	128
,	51
,	75
,	1
,	98
,	129
,	51
,	75
,	2
,	130
,	51
,	76
,	1
,	54
,	77
};

static unsigned char rle_stage2_values[] = {
	67
,	136
,	75
,	128
,	97
,	128
,	66
,	134
,	67
,	134
,	77
,	128
,	97
,	128
,	67
,	133
,	69
,	131
,	71
,	36
,	65
,	131
,	70
,	130
,	70
,	130
,	70
,	33
,	2
,	33
,	87
,	128
,	69
,	33
,	4
,	32
,	93
,	33
,	4
,	33
,	93
,	32
,	4
,	33
,	94
,	32
,	3
,	33
,	95
,	33
,	1
,	33
,	95
,	65
,	35
,	95
,	95
,	95
,	95
,	95
,	95
,	95
,	79
,	128
,	95
,	68
,	129
,	95
,	67
,	130
,	71
,	131
,	71
,	134
,	70
,	133
,	69
,	129
,	98
,	69
,	130
,	100
,	128
,	69
,	135
,	67
,	129
,	99
,	69
,	128
,	102
,	128
,	67
,	139
,	65
,	128
,	100
	};
	
static unsigned char rle_stage3_values[] = {
	129
,	68
,	34
,	18
,	33
,	67
,	128
,	98
,	130
,	68
,	33
,	18
,	33
,	67
,	129
,	97
,	128
,	70
,	33
,	18
,	33
,	68
,	130
,	71
,	33
,	18
,	34
,	78
,	33
,	19
,	33
,	78
,	34
,	18
,	33
,	78
,	34
,	18
,	33
,	79
,	33
,	18
,	33
,	79
,	33
,	7
,	34
,	7
,	33
,	80
,	32
,	7
,	34
,	7
,	33
,	80
,	32
,	7
,	34
,	7
,	33
,	79
,	33
,	18
,	34
,	78
,	33
,	19
,	33
,	78
,	34
,	18
,	33
,	78
,	34
,	18
,	34
,	77
,	34
,	19
,	33
,	78
,	33
,	19
,	33
,	68
,	133
,	67
,	33
,	19
,	33
,	68
,	100
,	132
,	33
,	19
,	33
,	68
,	104
,	130
,	19
,	34
,	67
,	106
,	128
,	20
,	33
,	67
};

static unsigned char rle_stage4_values[] = {
	31
,	31
,	31
,	31
,	31
,	31
,	31
,	26
,	35
,	31
,	1
,	32
,	132
,	31
,	32
,	129
,	32
,	129
,	32
,	128
,	30
,	32
,	131
,	33
,	128
,	30
,	32
,	129
,	35
,	128
,	30
,	32
,	128
,	33
,	128
,	33
,	128
,	31
,	36
,	128
,	31
,	1
,	131
,	31
,	31
,	31
,	31
,	31
,	31
,	31
,	31
,	31
,	2
};

static unsigned char rle_stage5_values[] = {
	1
,	32
,	67
,	128
,	99
,	128
,	78
,	128
,	105
,	1
,	32
,	67
,	129
,	97
,	129
,	78
,	129
,	104
,	1
,	33
,	67
,	131
,	80
,	130
,	102
,	1
,	33
,	77
,	128
,	75
,	135
,	2
,	33
,	74
,	131
,	76
,	133
,	2
,	33
,	75
,	132
,	75
,	132
,	1
,	34
,	76
,	129
,	78
,	131
,	1
,	33
,	95
,	130
,	1
,	34
,	94
,	130
,	2
,	33
,	94
,	130
,	2
,	33
,	94
,	130
,	1
,	33
,	95
,	130
,	1
,	33
,	95
,	130
,	1
,	33
,	95
,	130
,	1
,	32
,	95
,	131
,	1
,	33
,	93
,	132
,	1
,	33
,	75
,	128
,	79
,	133
,	2
,	33
,	73
,	131
,	74
,	136
,	2
,	33
,	73
,	131
,	73
,	129
,	103
,	2
,	33
,	72
,	132
,	71
,	130
,	104
,	3
,	32
,	71
,	134
,	70
,	128
,	106
};

static unsigned char rle_stage6_values[] = {
	105
,	144
,	117
,	131
,	66
,	138
,	114
,	132
,	69
,	139
,	97
,	130
,	98
,	140
,	71
,	159
,	64
,	128
,	67
,	159
,	131
,	64
,	159
,	159
,	159
,	159
,	159
,	159
,	159
,	159
,	153
,	64
,	159
,	132
,	66
,	159
,	131
,	68
,	149
,	64
,	139
,	66
,	147
,	69
,	138
,	64
,	140
,	100
,	131
,	66
,	146
,	98
,	129
,	106
,	146
,	116
,	144
,	117
,	144
,	105
};

static unsigned char rle_stage7_values[] = {
	99
,	128
,	77
,	131
,	65
,	130
,	72
,	33
,	99
,	128
,	78
,	129
,	65
,	131
,	71
,	34
,	98
,	129
,	78
,	135
,	70
,	35
,	98
,	128
,	80
,	131
,	64
,	128
,	72
,	34
,	98
,	128
,	80
,	132
,	73
,	34
,	98
,	128
,	81
,	129
,	75
,	34
,	98
,	128
,	82
,	128
,	75
,	34
,	97
,	129
,	94
,	35
,	130
,	95
,	35
,	129
,	95
,	65
,	34
,	129
,	95
,	65
,	34
,	128
,	95
,	66
,	34
,	128
,	95
,	66
,	34
,	129
,	95
,	66
,	33
,	129
,	95
,	66
,	33
,	131
,	69
,	37
,	83
,	34
,	98
,	128
,	67
,	34
,	3
,	33
,	81
,	35
,	98
,	129
,	65
,	33
,	6
,	33
,	80
,	35
,	99
,	128
,	65
,	32
,	2
,	97
,	3
,	33
,	79
,	35
,	99
,	128
,	65
,	32
,	1
,	99
,	3
,	32
,	78
,	36
,	99
,	128
,	65
,	32
,	1
,	100
,	2
,	32
,	77
,	37
};	

static unsigned char rle_stage8_values[] = {
	33
,	142
,	99
,	2
,	103
,	33
,	164
,	33
,	133
,	69
,	131
,	97
,	3
,	103
,	33
,	164
,	34
,	130
,	73
,	129
,	97
,	3
,	104
,	33
,	163
,	34
,	129
,	70
,	128
,	66
,	130
,	96
,	5
,	102
,	33
,	163
,	34
,	129
,	75
,	129
,	6
,	103
,	33
,	162
,	34
,	129
,	75
,	129
,	6
,	103
,	33
,	162
,	33
,	64
,	130
,	65
,	128
,	72
,	128
,	6
,	103
,	33
,	162
,	33
,	65
,	129
,	75
,	9
,	101
,	33
,	162
,	34
,	78
,	9
,	101
,	33
,	162
,	34
,	78
,	9
,	101
,	33
,	162
,	34
,	64
,	129
,	75
,	9
,	101
,	33
,	162
,	34
,	131
,	74
,	9
,	101
,	33
,	162
,	34
,	130
,	75
,	9
,	101
,	33
,	162
,	34
,	130
,	75
,	9
,	101
,	33
,	162
,	34
,	131
,	74
,	128
,	6
,	103
,	33
,	162
,	33
,	64
,	130
,	75
,	128
,	6
,	103
,	33
,	162
,	33
,	66
,	128
,	70
,	128
,	66
,	129
,	6
,	103
,	33
,	162
,	33
,	70
,	128
,	70
,	129
,	6
,	102
,	33
,	163
,	33
,	77
,	130
,	96
,	3
,	104
,	33
,	163
,	34
,	64
,	131
,	70
,	130
,	98
,	2
,	103
,	33
,	164
,	34
,	141
,	99
,	2
,	103
,	33
,	164
  };
