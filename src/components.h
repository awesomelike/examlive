#include<gtk/gtk.h>
#include<gtk/gtkx.h>

GtkWidget	*login_window;
GtkWidget	*login_fixed;
GtkWidget	*login_image_1;
GtkWidget	*login_button;
GtkWidget	*login_username;
GtkWidget	*login_password;
GtkBuilder	*builder; 
GtkWidget   *login_label_error;

//GTK professor main panel
GtkWidget *pr_window_panel;
GtkWidget *start_btn;
GtkWidget *pr_swap_panel;
GtkWidget *label_prof_name;
GtkWidget *label_prof_id;
GtkWidget *login_spinner;

//GTK professor create quiz
GtkWidget *entry_exam_title;
GtkWidget *combo_course;
GtkWidget *entry_combo;
GtkWidget *liststore2;
GtkWidget *label_num_questions;
GtkWidget *entry_question;
GtkWidget *grid_create_answers;
GtkWidget *entry_answer_a;
GtkWidget *entry_answer_b;
GtkWidget *entry_answer_c;
GtkWidget *entry_answer_d;
GtkWidget *radio_a;
GtkWidget *radio_b;
GtkWidget *radio_c;
GtkWidget *radio_d;
GtkWidget *btn_add_question;
GtkWidget *btn_save_exam;

//GTK professor create quiz
GtkWidget *combo_start_quiz;
GtkWidget *entry_combo_start_quiz;
GtkWidget *liststore3;
GtkWidget *btn_start_exam;

GtkWidget *st_window_panel;
GtkWidget *st_name;
GtkWidget *st_id;
GtkWidget *label_button_student;

//CSS values
GtkCssProvider  *provider;
GdkScreen       *screen;
GdkDisplay      *display;