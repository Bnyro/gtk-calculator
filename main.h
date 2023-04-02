#include <gtk/gtk.h>

void pack_number_button(GtkWidget *grid, int number);
void pack_button(GtkWidget *grid, char *text, int column, int row, int weight);
double calculate_expr(const char *expr);
void load_style();
