/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.c
 * Copyright (C) 2018 Massimo <dongmassimo@gmail.com>
 * 
 * soltant-uno is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * soltant-uno is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * This file is generated by ./share/cards_generate.c
 * This file defines the card set
 */

#define INCLUDE_CARDS 1
#ifndef INCLUDE_CARDS
enum{
	CARD_COLOUR_RED = 0, CARD_COLOUR_YELLOW, CARD_COLOUR_GREEN, CARD_COLOUR_BLUE, CARD_COLOUR_BLACK
};

enum{
	CARD_SKIP = 10, CARD_REVERSE, CARD_DRAW2, CARD_WILD, CARD_WILD_DRAW4
};

struct CARD{
	int colour, identity;
}Card[108] = 
{
	{CARD_COLOUR_RED, 0},
	{CARD_COLOUR_RED, 1},
	{CARD_COLOUR_RED, 2},
	{CARD_COLOUR_RED, 3},
	{CARD_COLOUR_RED, 4},
	{CARD_COLOUR_RED, 5},
	{CARD_COLOUR_RED, 6},
	{CARD_COLOUR_RED, 7},
	{CARD_COLOUR_RED, 8},
	{CARD_COLOUR_RED, 9},
	{CARD_COLOUR_RED, CARD_SKIP},
	{CARD_COLOUR_RED, CARD_REVERSE},
	{CARD_COLOUR_RED, CARD_DRAW2},
	{CARD_COLOUR_BLACK, CARD_WILD},
	{CARD_COLOUR_YELLOW, 0},
	{CARD_COLOUR_YELLOW, 1},
	{CARD_COLOUR_YELLOW, 2},
	{CARD_COLOUR_YELLOW, 3},
	{CARD_COLOUR_YELLOW, 4},
	{CARD_COLOUR_YELLOW, 5},
	{CARD_COLOUR_YELLOW, 6},
	{CARD_COLOUR_YELLOW, 7},
	{CARD_COLOUR_YELLOW, 8},
	{CARD_COLOUR_YELLOW, 9},
	{CARD_COLOUR_YELLOW, CARD_SKIP},
	{CARD_COLOUR_YELLOW, CARD_REVERSE},
	{CARD_COLOUR_YELLOW, CARD_DRAW2},
	{CARD_COLOUR_BLACK, CARD_WILD},
	{CARD_COLOUR_GREEN, 0},
	{CARD_COLOUR_GREEN, 1},
	{CARD_COLOUR_GREEN, 2},
	{CARD_COLOUR_GREEN, 3},
	{CARD_COLOUR_GREEN, 4},
	{CARD_COLOUR_GREEN, 5},
	{CARD_COLOUR_GREEN, 6},
	{CARD_COLOUR_GREEN, 7},
	{CARD_COLOUR_GREEN, 8},
	{CARD_COLOUR_GREEN, 9},
	{CARD_COLOUR_GREEN, CARD_SKIP},
	{CARD_COLOUR_GREEN, CARD_REVERSE},
	{CARD_COLOUR_GREEN, CARD_DRAW2},
	{CARD_COLOUR_BLACK, CARD_WILD},
	{CARD_COLOUR_BLUE, 0},
	{CARD_COLOUR_BLUE, 1},
	{CARD_COLOUR_BLUE, 2},
	{CARD_COLOUR_BLUE, 3},
	{CARD_COLOUR_BLUE, 4},
	{CARD_COLOUR_BLUE, 5},
	{CARD_COLOUR_BLUE, 6},
	{CARD_COLOUR_BLUE, 7},
	{CARD_COLOUR_BLUE, 8},
	{CARD_COLOUR_BLUE, 9},
	{CARD_COLOUR_BLUE, CARD_SKIP},
	{CARD_COLOUR_BLUE, CARD_REVERSE},
	{CARD_COLOUR_BLUE, CARD_DRAW2},
	{CARD_COLOUR_BLACK, CARD_WILD},
	{CARD_COLOUR_RED, 1},
	{CARD_COLOUR_RED, 2},
	{CARD_COLOUR_RED, 3},
	{CARD_COLOUR_RED, 4},
	{CARD_COLOUR_RED, 5},
	{CARD_COLOUR_RED, 6},
	{CARD_COLOUR_RED, 7},
	{CARD_COLOUR_RED, 8},
	{CARD_COLOUR_RED, 9},
	{CARD_COLOUR_RED, CARD_SKIP},
	{CARD_COLOUR_RED, CARD_REVERSE},
	{CARD_COLOUR_RED, CARD_DRAW2},
	{CARD_COLOUR_BLACK, CARD_WILD_DRAW4},
	{CARD_COLOUR_YELLOW, 1},
	{CARD_COLOUR_YELLOW, 2},
	{CARD_COLOUR_YELLOW, 3},
	{CARD_COLOUR_YELLOW, 4},
	{CARD_COLOUR_YELLOW, 5},
	{CARD_COLOUR_YELLOW, 6},
	{CARD_COLOUR_YELLOW, 7},
	{CARD_COLOUR_YELLOW, 8},
	{CARD_COLOUR_YELLOW, 9},
	{CARD_COLOUR_YELLOW, CARD_SKIP},
	{CARD_COLOUR_YELLOW, CARD_REVERSE},
	{CARD_COLOUR_YELLOW, CARD_DRAW2},
	{CARD_COLOUR_BLACK, CARD_WILD_DRAW4},
	{CARD_COLOUR_GREEN, 1},
	{CARD_COLOUR_GREEN, 2},
	{CARD_COLOUR_GREEN, 3},
	{CARD_COLOUR_GREEN, 4},
	{CARD_COLOUR_GREEN, 5},
	{CARD_COLOUR_GREEN, 6},
	{CARD_COLOUR_GREEN, 7},
	{CARD_COLOUR_GREEN, 8},
	{CARD_COLOUR_GREEN, 9},
	{CARD_COLOUR_GREEN, CARD_SKIP},
	{CARD_COLOUR_GREEN, CARD_REVERSE},
	{CARD_COLOUR_GREEN, CARD_DRAW2},
	{CARD_COLOUR_BLACK, CARD_WILD_DRAW4},
	{CARD_COLOUR_BLUE, 1},
	{CARD_COLOUR_BLUE, 2},
	{CARD_COLOUR_BLUE, 3},
	{CARD_COLOUR_BLUE, 4},
	{CARD_COLOUR_BLUE, 5},
	{CARD_COLOUR_BLUE, 6},
	{CARD_COLOUR_BLUE, 7},
	{CARD_COLOUR_BLUE, 8},
	{CARD_COLOUR_BLUE, 9},
	{CARD_COLOUR_BLUE, CARD_SKIP},
	{CARD_COLOUR_BLUE, CARD_REVERSE},
	{CARD_COLOUR_BLUE, CARD_DRAW2},
	{CARD_COLOUR_BLACK, CARD_WILD_DRAW4},
};
#endif