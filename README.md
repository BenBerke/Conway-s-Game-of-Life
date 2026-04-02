# Conway's Game of Life

A small **for-fun Conway’s Game of Life project** I made in **C** using **SDL3** and **SDL3_ttf**.

I mainly built this to practise working with grids, simulation logic, input handling, rendering, and a simple history system in C. You can click to create patterns, then step forwards and backwards through generations.

## Features

- Click to turn cells on and off
- Step forwards through generations
- Step backwards through previous states
- Custom history system for undo-like navigation
- Simple text UI rendered with SDL_ttf

## Controls

- **Left Click** — toggle a cell
- **Right Arrow** — step forward one generation
- **Left Arrow** — step backward through history
- **Escape** — quit

## Tech

- **C**
- **SDL3**
- **SDL3_ttf**
- **CMake**

## What I Practised

This project was a good excuse to mess around with:

- 2D grid-based logic
- Conway's Game of Life rules
- dynamic memory allocation in C
- storing previous program states
- mouse and keyboard input
- basic rendering with SDL3
- text rendering with SDL_ttf

## How It Works

The program starts with an empty grid. You can click cells to build an initial pattern, then use the arrow keys to move through generations.

When moving forward, the program either:
- loads the next saved state if it already exists, or
- computes a new generation and stores it in history

When moving backward, it restores a previous grid state from the history list.