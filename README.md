# Codingame Bot Challenge - Ocean Scanning

Welcome to the Codingame Bot Challenge - Ocean Scanning! This coding challenge is based on a system of leagues where players compete to scan the most fish in the ocean using their drones. The goal is to explore the ocean floor, understand marine life, and outscore your opponents.
I participated in this challenge for my school, 1337, securing the second position globally. My individual rank is 77 out of 4669 participants.

## Challenge Overview

### Goal
Win more points than your opponent by scanning the most fish.

### Rules
- The game is played turn by turn.
- Each player controls two drones to explore the ocean floor and scan fish.
- Drones emit light to scan fish within a radius. Increasing light power drains the battery.
- Resurface to save scans and score points.
- Encounter depth monsters that chase drones if blinded by lights.

### Game Elements
- **Drones:** Move, emit light, and resurface. Limited battery capacity.
- **Fish:** Move within habitat zones. Scan for points and bonuses.
- **Depth Monsters:** Lurk and chase drones if detected.

### Scoring
Points awarded for scanned fish types, bonuses for scanning all of one type or color.
- Type 0: 1 point (2 if first to save)
- Type 1: 2 points (4 if first to save)
- Type 2: 3 points (6 if first to save)
- All fish of one color: 3 points (6 if first to save)
- All fish of one type: 4 points (8 if first to save)

### Victory Conditions
- Game reaches 200 turns.
- A player earns enough points to be unbeatable.
- Both players save scans of all remaining fish.

### Defeat Conditions
- Program doesn't return a valid command within the given time for each drone.

## Game Protocol

### Initialization Input
- Number of creatures and details (color, type, id) for each creature.

### Input for One Game Turn
- Scores, saved scans, drone details, creature positions, and radar information.

### Output
- Instructions for each drone (MOVE x y light or WAIT light).

## Get Started
Clone the repository, and dive into the exciting world of ocean scanning! Good luck, and may the best coder win! 🌊🤖
