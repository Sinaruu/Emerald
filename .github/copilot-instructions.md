# Copilot Instructions

## General Guidelines
- Functions should validate pointer parameters (not NULL) and the operation parameter.
- Use defensive programming practices to ensure code robustness.

## Code Style
- Follow consistent indentation and spacing.
- Maintain existing formatting conventions (code blocks, emphasis, links).

## Project-Specific Rules
- The `vigenereFile` function reads an input file and writes an output file applying the Vigenère cipher with the key repeated across the visible ASCII range.
- Non-visible characters in the input file should remain unchanged.