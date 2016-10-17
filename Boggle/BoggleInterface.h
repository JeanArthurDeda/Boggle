#pragma once

#ifndef BOGGLE_INTERFACE_H__
#define BOGGLE_INTERFACE_H__

#include "BoggleSolver.h"

// ===========================================================
// Incorporates the agreed-upon interface for Boggle Challange
// ===========================================================

// defined in BoggleSolver.h
//struct Results
//{
//	const char* const* Words;												// pointers to unique found words, each terminated by a non-alpha char
//	unsigned           Count;												// number of words found
//	unsigned           Score;												// total score
//	void*              UserData;											// ignored
//};

// input dictionary is a file with one word per line
// It will also generate a dictionary partition
void LoadDictionary(const char* path);

// this func may be called multiple times
// board: will be exactly width * height chars, and 'q' represents the 'qu' Boggle cube
Results FindWords(const char* board, unsigned width, unsigned height);

// 'results' is identical to what was returned from FindWords
void FreeWords(Results results);

// Free the loaded dictionarey and it's partition
void FreeDictionary(); // << TODO

#endif // !BOGGLE_INTERFACE_H__

