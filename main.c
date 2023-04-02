#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

#include "main.h"

GtkEntryBuffer *buffer;

int calculate_expr(const char *expr) {
  return 0;
}

static void calculate (GtkWidget *widget, gpointer data)
{
  g_print ("Hello World\n");
}

static void update_text (GtkWidget *widget, gpointer data)
{
  const char *label = gtk_button_get_label(GTK_BUTTON(widget));
  const char *current_text = gtk_entry_buffer_get_text(buffer);

  if (strcmp(label, "C") == 0) {
    gtk_entry_buffer_set_text(buffer, "", -1);
  } else if (strcmp(label, "=") == 0) {
    char* str;
    int result = calculate_expr(current_text);
    asprintf(&str, "%i", result);
    gtk_entry_buffer_set_text(buffer, str, -1);
  } else {
    char buf[256];
    snprintf(buf, sizeof(buf), "%s%s", current_text, label);
    gtk_entry_buffer_set_text(buffer, buf, -1);
  }
}

static void activate (GtkApplication *app, gpointer user_data)
{
  GtkWidget *window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");
  gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

  GtkWidget *main_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

  buffer = gtk_entry_buffer_new ("", -1);
  GtkWidget *entry = gtk_entry_new_with_buffer(buffer);
  gtk_editable_set_editable(GTK_EDITABLE(entry), FALSE);

  GtkWidget *heading = gtk_label_new("Calculator");

  GtkWidget *number_grid = gtk_grid_new();
  gtk_widget_set_valign(number_grid, GTK_ALIGN_FILL);
  gtk_widget_set_halign(number_grid, GTK_ALIGN_FILL);

  for (int i = 1; i <= 9; i++)
  {
    pack_number_button(number_grid, i);
  }

  char *ops = "/x-+";

  for (int i = 0; i < strlen(ops); i++) {
    char *s = ops[i];
    pack_button(number_grid, &s, 4, i, 1);
  }

  pack_button(number_grid, "C", 0, 0, 3);
  pack_button(number_grid, "0", 0, 5, 2);
  pack_button(number_grid, ".", 2, 5, 1);
  pack_button(number_grid, "=", 4, 5, 1);

  gtk_box_append(GTK_BOX(main_container), heading);
  gtk_box_append(GTK_BOX(main_container), entry);
  gtk_box_append(GTK_BOX(main_container), number_grid);

  gtk_window_set_child (GTK_WINDOW (window), main_container);
  gtk_window_present (GTK_WINDOW (window));
}

void pack_number_button(GtkWidget *grid, int number)
{
  char* str;
  asprintf(&str, "%i", number);
  pack_button(grid, str, (number - 1) % 3, (9 - number) / 3 + 1, 1);
}

void pack_button(GtkWidget *grid, char *text, int column, int row, int weight)
{
  GtkWidget *button = gtk_button_new_with_label(text);
  g_signal_connect (button, "clicked", G_CALLBACK (update_text), NULL);
  gtk_grid_attach(GTK_GRID(grid), button, column, row, weight, 1);
}

void load_style()
{
  GtkCssProvider *cssProvider = gtk_css_provider_new();
  gtk_css_provider_load_from_path(cssProvider, "theme.css");
  gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                               GTK_STYLE_PROVIDER(cssProvider),
                               GTK_STYLE_PROVIDER_PRIORITY_USER);
}

int main (int argc, char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("com.bnyro.calculate", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
