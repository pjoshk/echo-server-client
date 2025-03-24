#include <ncurses.h>
#include <form.h>
#include <string.h>
#include <unistd.h>

void position_input_title(WINDOW *win, int starty, int startx, int width, char *string, chtype color);

WINDOW *create_newwin(int height, int width, int starty, int startx) {
  WINDOW *local_win;

  local_win = newwin(height, width, starty, startx);
  box(local_win, 0, 0); /* 0, 0 gives default characters for the vertical and horizontal lines */
  wrefresh(local_win);  /* Show that box 		*/

  return local_win;
}


int renderWindow(int client_fd) {
  FIELD *field[2];
  FORM *my_form;
  WINDOW *my_form_win;
  WINDOW *my_display_win;
  char client_msg[1024];
  int ch, rows, cols;

  /* Initialize curses */
  initscr();
  start_color();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  /* Initialize few color pairs */
  init_pair(1, COLOR_YELLOW, COLOR_BLACK);

  /* Initialize the fields */
  field[0] = new_field(1, COLS - 10, 0, 0, 0, 0);
  field[1] = NULL;

  /* Set field options */
  set_field_back(field[0], A_STANDOUT);
  field_opts_off(field[0], O_AUTOSKIP); /* Don't go to next field when this */
                                        /* Field is filled up 		*/

  /* Create the form and post it */
  my_form = new_form(field);

  /* Calculate the area required for the form */
  scale_form(my_form, &rows, &cols);

  /* Create the window to be associated with the form */
  my_form_win = newwin(rows + 3, cols + 4, LINES-6, 4);
  keypad(my_form_win, TRUE);


  /* Set main window and sub window */
  set_form_win(my_form, my_form_win);
  set_form_sub(my_form, derwin(my_form_win, rows, cols, 2, 2));

  /* Print a border around the main window and print a title */
  box(my_form_win, 0, 0);
  position_input_title(my_form_win, 1, 0, cols + 4, "My Form", COLOR_PAIR(1));

  post_form(my_form);
  wrefresh(my_form_win);

  mvprintw(LINES - 2, 0, "Use UP, DOWN arrow keys to switch between fields");
  refresh();

  /* Create the window to be associated with the display */
  my_display_win = create_newwin(LINES-6, COLS-6, 0, 4);
  refresh();

  form_driver(my_form, REQ_BEG_LINE);
  /* Loop through to get user requests */
  int cur_pos = 0;
  while ((ch = wgetch(my_form_win)) != KEY_F(1)) {
    switch (ch) {
    case KEY_BACKSPACE:
      form_driver(my_form, REQ_DEL_PREV);
      if(cur_pos > 0) {
        cur_pos -= 1;
      }
      break;
    case KEY_DOWN:
      /* Go to next field */
      form_driver(my_form, REQ_NEXT_FIELD);
      /* Go to the end of the present buffer */
      /* Leaves nicely at the last character */
      form_driver(my_form, REQ_END_LINE);
      break;
    case KEY_UP:
      /* Go to previous field */
      form_driver(my_form, REQ_PREV_FIELD);
      form_driver(my_form, REQ_END_LINE);
      break;

      // when the client presses enter
      case '\n': 
        form_driver(my_form, REQ_CLR_FIELD);
        client_msg[cur_pos] = '\n';
        client_msg[cur_pos+1] = '\0';

        // send message to the server
        write(client_fd, client_msg, sizeof(client_msg));
        
        cur_pos = 0;
        break;
    default:
      /* If this is a normal character, it gets */
      /* Printed				  */
      form_driver(my_form, ch);
      client_msg[cur_pos]  = (char)ch;
      cur_pos++;
      break;
    }
  }

  /* Un post form and free the memory */
  unpost_form(my_form);
  free_form(my_form);
  free_field(field[0]);
  free_field(field[1]);

  endwin();
  return 0;
}

void position_input_title(WINDOW *win, int starty, int startx, int width,
                     char *string, chtype color) {
  int length, x, y;
  float temp;

  if (win == NULL)
    win = stdscr;
  getyx(win, y, x);
  if (startx != 0)
    x = startx;
  if (starty != 0)
    y = starty;
  if (width == 0)
    width = 80;

  length = strlen(string);
  x = startx + 2;
  y = starty - 1;
  wattron(win, color);
  mvwprintw(win, y, x, "%s", string);
  wattroff(win, color);
  refresh();
}
