#ifndef TRADING_STUFF_TEXT_H_
#define TRADING_STUFF_TEXT_H_

// In-place tokenizer for line-based text formats (level/.def files). Skips leading whitespace,
// null-terminates the next whitespace-delimited token, advances *cursor past it. NULL at end.
char *text_next_token(char **cursor);

// Trims surrounding spaces, tabs, CR and double-quotes in place; returns the inner pointer.
char *text_trim(char *s);

#endif // TRADING_STUFF_TEXT_H_
