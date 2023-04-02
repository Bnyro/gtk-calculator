#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

#include "main.h"

GtkEntryBuffer *buffer;

double calculate_expr(const char *expr)
{
   char postfix[strlen(expr) + 1]; 
   convert(expr, postfix);
   g_print(postfix);
   int res = evaluate(postfix);
   g_print("%d", res);
   return res;
}

static void update_text(GtkWidget *widget, gpointer data)
{
  const char *label = gtk_button_get_label(GTK_BUTTON(widget));
  const char *current_text = gtk_entry_buffer_get_text(buffer);

  if (strcmp(label, "C") == 0)
  {
    gtk_entry_buffer_set_text(buffer, "", -1);
  }
  else if (strcmp(label, "=") == 0)
  {
    char *str;
    int result = calculate_expr(current_text);
    asprintf(&str, "%d", result);
    gtk_entry_buffer_set_text(buffer, str, -1);
  }
  else
  {
    char buf[256];
    snprintf(buf, sizeof(buf), "%s%s", current_text, label);
    gtk_entry_buffer_set_text(buffer, buf, -1);
  }
}

static void activate(GtkApplication *app, gpointer user_data)
{
  load_style();

  GtkWidget *window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Calculator");
  gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);

  GtkWidget *main_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_widget_add_css_class(main_container, "main-container");

  buffer = gtk_entry_buffer_new("", -1);
  GtkWidget *entry = gtk_entry_new_with_buffer(buffer);
  gtk_editable_set_editable(GTK_EDITABLE(entry), FALSE);
  gtk_widget_add_css_class(entry, "expression");

  GtkWidget *heading = gtk_label_new("Calculator");
  gtk_widget_add_css_class(heading, "heading");

  GtkWidget *number_grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(number_grid), 3);
  gtk_grid_set_column_spacing(GTK_GRID(number_grid), 3);

  for (int i = 1; i <= 9; i++)
  {
    pack_number_button(number_grid, i);
  }

  char *ops = "/*-+";

  for (int i = 0; i < strlen(ops); i++)
  {
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

  gtk_window_set_child(GTK_WINDOW(window), main_container);
  gtk_window_present(GTK_WINDOW(window));
}

void pack_number_button(GtkWidget *grid, int number)
{
  char *str;
  asprintf(&str, "%i", number);
  pack_button(grid, str, (number - 1) % 3, (9 - number) / 3 + 1, 1);
}

void pack_button(GtkWidget *grid, char *text, int column, int row, int weight)
{
  GtkWidget *button = gtk_button_new_with_label(text);
  gtk_widget_set_vexpand(button, true);
  gtk_widget_set_hexpand(button, true);
  g_signal_connect(button, "clicked", G_CALLBACK(update_text), NULL);
  gtk_grid_attach(GTK_GRID(grid), button, column, row, weight, 1);
}

void load_style()
{
  GtkCssProvider *cssProvider = gtk_css_provider_new();
  gtk_css_provider_load_from_path(cssProvider, "style.css");
  gtk_style_context_add_provider_for_display(gdk_display_get_default(),
                                             GTK_STYLE_PROVIDER(cssProvider),
                                             GTK_STYLE_PROVIDER_PRIORITY_USER);
}

int main(int argc, char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new("com.bnyro.calculator", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}

// char stack
char stack[];
int top = -1;

void push(char item)
{
  stack[++top] = item;
}
char pop()
{
  return stack[top--];
}

// returns precedence of operators
int precedence(char symbol)
{
  switch (symbol)
  {
  case '+':
  case '-':
    return 2;
    break;
  case '*':
  case '/':
    return 3;
    break;
  case '^':
    return 4;
    break;
  case '(':
  case ')':
  case '#':
    return 1;
    break;
  }
}

// check whether the symbol is operator?
int isOperator(char symbol)
{
  switch (symbol)
  {
  case '+':
  case '-':
  case '*':
  case '/':
  case '^':
  case '(':
  case ')':
    return 1;
    break;
  default:
    return 0;
  }
}
// converts infix expression to postfix
void convert(char *infix, char *postfix)
{
  int i, symbol, j = 0;
  stack[++top] = '#';
  for (i = 0; i < strlen(infix); i++)
  {
    symbol = infix[i];
    if (isOperator(symbol) == 0)
    {
      postfix[j] = symbol;
      j++;
    }
    else
    {
      if (symbol == '(')
      {
        push(symbol);
      }
      else
      {
        if (symbol == ')')
        {
          while (stack[top] != '(')
          {
            postfix[j] = pop();
            j++;
          }
          pop(); // pop out (.
        }
        else
        {
          if (precedence(symbol) > precedence(stack[top]))
          {
            push(symbol);
          }
          else
          {
            while (precedence(symbol) <= precedence(stack[top]))
            {
              postfix[j] = pop();
              j++;
            }
            push(symbol);
          }
        }
      }
    }
  }
  while (stack[top] != '#')
  {
    postfix[j] = pop();
    j++;
  }
  postfix[j] = '\0'; // null terminate string.
}
// int stack
int stack_int[25];
int top_int = -1;

void push_int(int item)
{
  stack_int[++top_int] = item;
}

char pop_int()
{
  return stack_int[top_int--];
}
// evaluates postfix expression
int evaluate(char *postfix)
{
  char ch;
  int i = 0, operand1, operand2;

  while ((ch = postfix[i++]) != '\0')
  {
    if (isdigit(ch))
    {
      push_int(ch - '0'); // Push the operand
    }
    else
    {
      // Operator,pop two  operands
      operand2 = pop_int();
      operand1 = pop_int();
      switch (ch)
      {
      case '+':
        push_int(operand1 + operand2);
        break;
      case '-':
        push_int(operand1 - operand2);
        break;
      case '*':
        push_int(operand1 * operand2);
        break;
      case '/':
        push_int(operand1 / operand2);
        break;
      }
    }
  }
  return stack_int[top_int];
}