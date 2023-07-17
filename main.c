#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

char* front;
char* back;
char* front_with_clear;
char* back_with_clear;
unsigned int size;
unsigned int width;
unsigned int height;

void init_cells();
void print_cells();
void update_cells();

struct winsize get_winsize() {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return w;
}

inline int mod(int a, int base){    
  return ((a % base) + base) % base;
}


int main() {
	srand(time(NULL));
	struct winsize w = get_winsize();
	width = w.ws_col;
	height = w.ws_row;
	size = width*height;
	front_with_clear = malloc(sizeof(char)*(size + 4));
	back_with_clear = malloc(sizeof(char)*(size + 4));
	back_with_clear[0] = '\033';
	back_with_clear[1] = '[';
	back_with_clear[2] = '2';
	back_with_clear[3] = 'J';
	front_with_clear[0] = '\033';
	front_with_clear[1] = '[';
	front_with_clear[2] = '2';
	front_with_clear[3] = 'J';
	front = &front_with_clear[4];
	back  = &back_with_clear[4];
	init_cells();
	while (1) {
		update_cells();
		print_cells();
	}
	free(front_with_clear);
	free(back_with_clear);
	return 0;
}

inline void update_cells() {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			unsigned int neighbours = 
				  (front[mod(y+1, height)*width + mod(x+1, width)] == '#')
				+ (front[mod(y+1, height)*width +     x          ] == '#')
				+ (front[mod(y+1, height)*width + mod(x-1, width)] == '#')
				+ (front[    y           *width + mod(x+1, width)] == '#')
				+ (front[    y           *width + mod(x-1, width)] == '#')
				+ (front[mod(y-1, height)*width + mod(x+1, width)] == '#')
				+ (front[mod(y-1, height)*width +     x          ] == '#')
				+ (front[mod(y-1, height)*width + mod(x-1, width)] == '#');
			back[y*width+x] = (neighbours == 3 || (neighbours == 2 && front[y*width+x] == '#'))*('#' - ' ') + ' ';
		}
	}
	char* temp = front;
	front = back;
	back = temp;
};

void init_cells() {
	for (int i = 0; i < size; i++) {
		front[i] = (rand() & 1)*('#' - ' ') + ' ';
	}
}

inline void print_cells() {
	write(STDOUT_FILENO, front_with_clear, size + 4);
}
