#include <stdio.h>

enum{
	CARD_COLOUR_RED = 0, CARD_COLOUR_YELLOW, CARD_COLOUR_GREEN, CARD_COLOUR_BLUE, CARD_COLOUR_BLACK
};

enum{
	CARD_SKIP = 10, CARD_REVERSE, CARD_DRAW2, CARD_WILD, CARD_WILD_DRAW4
};


struct CARD{
	int colour, identity;
}Card[108] = {};

void print(){
	printf("enum{\n\tCARD_COLOUR_RED = 0, CARD_COLOUR_YELLOW, CARD_COLOUR_GREEN, CARD_COLOUR_BLUE, CARD_COLOUR_BLACK\n};\n\n");
	printf("enum{\n\tCARD_SKIP = 10, CARD_REVERSE, CARD_DRAW2, CARD_WILD, CARD_WILD_DRAW4\n};\n\n");
	printf("struct CARD{\n\tint colour, identity;\n}Card[108] = \n{\n");

	for(int i=0;i<108;++i){
		printf("\t{");
		switch(Card[i].colour){
			case CARD_COLOUR_RED:
				printf("CARD_COLOUR_RED, ");
				break;
			case CARD_COLOUR_YELLOW:
				printf("CARD_COLOUR_YELLOW, ");
				break;
			case CARD_COLOUR_GREEN:
				printf("CARD_COLOUR_GREEN, ");
				break;
			case CARD_COLOUR_BLUE:
				printf("CARD_COLOUR_BLUE, ");
				break;
			case CARD_COLOUR_BLACK:
				printf("CARD_COLOUR_BLACK, ");
				break;
		}

		if(Card[i].identity < 10){
			printf("%d},\n", Card[i].identity);
		}else{
			switch(Card[i].identity){
				case CARD_SKIP:
					printf("CARD_SKIP},\n");
					break;
				case CARD_REVERSE:
					printf("CARD_REVERSE},\n");
					break;
				case CARD_DRAW2:
					printf("CARD_DRAW2},\n");
					break;
				case CARD_WILD:
					printf("CARD_WILD},\n");
					break;
				case CARD_WILD_DRAW4:
					printf("CARD_WILD_DRAW4},\n");
					break;
			}
		}
	}

	printf("};\n");
}

int main(){
	for(int i=0;i<13;++i) Card[i] = (struct CARD){CARD_COLOUR_RED, i};
	Card[13] = (struct CARD){CARD_COLOUR_BLACK, CARD_WILD};

	for(int i=14;i<27;++i) Card[i] = (struct CARD){CARD_COLOUR_YELLOW, i-14};
	Card[27] = (struct CARD){CARD_COLOUR_BLACK, CARD_WILD};

	for(int i=28;i<41;++i) Card[i] = (struct CARD){CARD_COLOUR_GREEN, i-28};
	Card[41] = (struct CARD){CARD_COLOUR_BLACK, CARD_WILD};

	for(int i=42;i<55;++i) Card[i] = (struct CARD){CARD_COLOUR_BLUE, i-42};
	Card[55] = (struct CARD){CARD_COLOUR_BLACK, CARD_WILD};

	for(int i=56;i<68;++i) Card[i] = (struct CARD){CARD_COLOUR_RED, i-55};
	Card[68] = (struct CARD){CARD_COLOUR_BLACK, CARD_WILD_DRAW4};

	for(int i=69;i<81;++i) Card[i] = (struct CARD){CARD_COLOUR_YELLOW, i-68};
	Card[81] = (struct CARD){CARD_COLOUR_BLACK, CARD_WILD_DRAW4};

	for(int i=82;i<94;++i) Card[i] = (struct CARD){CARD_COLOUR_GREEN, i-81};
	Card[94] = (struct CARD){CARD_COLOUR_BLACK, CARD_WILD_DRAW4};

	for(int i=95;i<107;++i) Card[i] = (struct CARD){CARD_COLOUR_BLUE, i-94};
	Card[107] = (struct CARD){CARD_COLOUR_BLACK, CARD_WILD_DRAW4};

	print();
}
