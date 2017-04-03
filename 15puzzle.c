/*
 * 15puzzle.c - projekt 2 do predmetu GUX
 * autor: Frantisek Matecny, xmatec00
 */

#include <gtk/gtk.h>

#ifdef GTK3
GtkWidget *table;
#else
GtkTable *table;
#endif

GtkWidget *mainWindow;

GtkWidget *buttons[15];
unsigned left, right, top, bottom;
int p_empty = 15;
int p_current = 0;
int p_rand[16];
int count = 0;
GtkWidget *status;

/* 
 * Callback funckia - vyvola informativne modalne okno 
 */
static void aboutCB(GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog;
	
	dialog = gtk_message_dialog_new(GTK_WINDOW(mainWindow),
												GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
												GTK_MESSAGE_INFO, GTK_BUTTONS_OK, 
												"Hra - 15 Puzzle\n\nCielom hry je\n usporiadat cisla tak,\n aby boli v poradi");
												
	gtk_window_set_title(GTK_WINDOW(dialog), "About");
	
	/* zobrazenie modalneho dialogu */
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
		
}


/* 
 * Prehodenie tlacitiek 
 */
void swap_buttons(int i)
{
	unsigned l,t;

	g_object_ref (buttons[p_rand[i]]);
	g_object_ref (buttons[p_rand[i+1]]);
	
	gtk_container_remove (GTK_CONTAINER(table), buttons[p_rand[i]]);
	gtk_container_remove (GTK_CONTAINER(table), buttons[p_rand[i+1]]);

	l = i%4;
	t = (i - l)/4;
	gtk_table_attach_defaults (table, buttons[p_rand[i+1]], l, l+1, t, t+1);
	
	i++;
	l = i%4;
	t = (i - l)/4;
	gtk_table_attach_defaults (table, buttons[p_rand[i-1]], l, l+1, t, t+1);		
}

/* 
 * Kontrola riesitelnosti ulohy
 * Neriesitelnu ulohu upravy na riesitelnu 
 */
void make_solvable()
{
	int inversions = 0;
	int index = 0;
	
	for (int i = 0; i < 16; i++)
	{
		for (int j = i+1; j < 16; j++)
		{
			if (p_rand[i] > p_rand[j] && p_rand[i] != -1 && p_rand[j] != -1)
			{
				inversions++;
				index = i;
			}
		}
	}
	
	inversions += (p_empty/4) +1;

	//ak je uloha neriesitelna, prehod tlacitka
	if(inversions%2 != 0)
		swap_buttons(index);
}



/* 
 * Premiesanie tlacitiek 
 */
static void shuffle(GtkWidget *widget, gpointer data)
{
	int rand, l, t;
	gboolean is_same;
	count = 0;
	
	
	for (int i = 0; i < 15; i++)
	{
		g_object_ref (buttons[i]);
		gtk_container_remove (GTK_CONTAINER(table), buttons[i]);
		p_rand[i] = -1;
	}
	p_rand[15] = -1;

	for (int i = 0; i < 15; i++)
	{
		is_same = TRUE;
		while (is_same)
		{
			is_same = FALSE;
			rand = g_rand_int_range (g_rand_new (), 0,  16);				
			if (p_rand[rand] < i && p_rand[rand] != -1)
				is_same = TRUE;	
		}
		
		p_rand[rand] = i;
		l = rand%4;
		t = (rand - l) / 4;
		gtk_table_attach_defaults (table, buttons[i], l, l+1, t, t+1);
	}

	int i;
	for (i = 0; p_rand[i] != -1; i++);
	p_empty = i;
	
	//kontrola riesitelnosti ulohy
	make_solvable();
	
	gtk_statusbar_push (GTK_STATUSBAR(status), 0, g_strdup_printf("Number of moves: %d", count));
}

/*
 * Kontrola ci je uloha vyriesena
 */
gboolean is_win()
{
	if (p_empty == 15)
	{
		for (int i = 0; i < 15; i++)
		{
			gtk_container_child_get(GTK_CONTAINER(table), buttons[i],
			"left-attach", &left, "right-attach", &right,
			"top-attach", &top, "bottom-attach", &bottom, NULL);
			
			if (i != left+top*4)
				return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}


/*
 * Premiestnenie tlacitka
 */
static void move(GtkWidget *widget, gpointer data)
{
	gtk_container_child_get(GTK_CONTAINER(table), widget,
		"left-attach", &left, "right-attach", &right,
		"top-attach", &top, "bottom-attach", &bottom, NULL);
	
	p_current = left+top*4;
	
	if( (p_current-1 == p_empty && p_current%4 != 0) ||
		(p_current+1 == p_empty && p_empty%4 != 0) ||
		p_current-4 == p_empty || 
		p_current+4 == p_empty)
		{	
			unsigned l = p_empty%4;
			unsigned t = (p_empty - l) / 4;
			
			//g_printf("%u,%u,%u,%u - %u\n", l, l+1, t, t+1, l+t*4);
			gtk_container_remove (GTK_CONTAINER(table), widget);
	
			gtk_table_attach_defaults (table, widget, l, l+1, t, t+1);
			p_empty = p_current;
			count++;
			gtk_statusbar_push (GTK_STATUSBAR(status), 0, g_strdup_printf("Number of moves: %d", count));
		}
	
	if (is_win())
	{
		gtk_statusbar_push (GTK_STATUSBAR(status), 0, g_strdup_printf("YOU WIN!\tScore: %d", count));
	}
		
		
}

/*
 * Osetrenie udalosti od spravcu okien - zatvorenie okna 
 */
static gboolean close_event(GtkWidget*widget, GdkEvent *event, gpointer data)
{
	return FALSE; /* emit destroy signal */
}

/* 
 * Akcna funkcia ukoncenia aplikacie 
 */
static void destroy(GtkWidget *widget, gpointer data)
{
	gtk_main_quit();
}

/* 
 * Hlavna funkcia programu 
 */
int main(int argc, char **argv)
{
	GtkWidget *box;
	GtkWidget *menubar, *file, *about, *filemenu, *aboutmenu;
	GtkAccelGroup *accel_group;
	
	/* inicializacia gtk */
	gtk_init(&argc, &argv);
	
	/* načtení konfiguračního souboru – volitelné, od 3.0 zrušeno */
	#ifndef GTK3
	gtk_rc_parse("move.rc");
	#endif
	
	
	/* vytvorenie hlavneho okna aplikacie */
	mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_name(mainWindow, "mainWindow");
	gtk_container_set_border_width(GTK_CONTAINER(mainWindow), 10);
	gtk_window_set_resizable (GTK_WINDOW(mainWindow), FALSE);
	
	/* nastavenie zpracovania udalosti WM_DELETE */
	g_signal_connect(G_OBJECT(mainWindow), "delete_event", G_CALLBACK(close_event), NULL);
	
	/* nastavenie zpracovania signalu destroy */
	g_signal_connect(G_OBJECT(mainWindow), "destroy", G_CALLBACK(destroy), NULL);

	/* box pre menu a table */
	#ifdef GTK3
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
	#else
	box = gtk_vbox_new(GTK_ORIENTATION_HORIZONTAL, 6);
	#endif
	
	/* menu bar */
	menubar = gtk_menu_bar_new();
	accel_group = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(mainWindow), accel_group);
	gtk_box_pack_start(GTK_BOX(box), menubar,FALSE, FALSE, 0);

	/* tlacitka v menubar*/
	file = gtk_menu_item_new_with_mnemonic("_File");
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file);

	about = gtk_menu_item_new_with_mnemonic("_About");
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), about);

	/* roletove menu */
	filemenu = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (file), filemenu);

	aboutmenu = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (about), aboutmenu);

	/* tlacitka v rolete */
	GtkWidget *b_new;
	b_new = gtk_menu_item_new_with_mnemonic("_New");
	//gtk_widget_add_accelerator(b_new,"activate", accel_group, GDK_A, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), b_new);
	g_signal_connect(G_OBJECT(b_new),"activate", G_CALLBACK(shuffle), NULL);

	GtkWidget *b_quit;
	b_quit = gtk_menu_item_new_with_mnemonic("_Quit");
	//gtk_widget_add_accelerator(b_quit,"activate", accel_group, GDK_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), b_quit);
	g_signal_connect(G_OBJECT(b_quit),"activate", G_CALLBACK(destroy), NULL);

	GtkWidget *b_about;
	b_about = gtk_menu_item_new_with_mnemonic("_About");
	//gtk_widget_add_accelerator(b_about,"activate", accel_group, GDK_a, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_menu_shell_append(GTK_MENU_SHELL(aboutmenu), b_about);
	g_signal_connect(G_OBJECT(b_about),"activate", G_CALLBACK(aboutCB), NULL);


	#ifdef GTK3
	gtk_widget_add_accelerator(b_new,"activate", accel_group, GDK_KEY_n, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(b_quit,"activate", accel_group, GDK_KEY_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(b_about,"activate", accel_group, GDK_KEY_a, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	#endif


	gtk_container_add (GTK_CONTAINER (mainWindow), box);

	/* hracska plocha */
	table = gtk_table_new(4, 4, TRUE);

	/* vytvorenie tlacitiek */
	for (int i = 0; i < 15; i++)
	{
		buttons[i] = gtk_button_new_with_label(g_strdup_printf("%d", i+1));
		gtk_widget_set_size_request(buttons[i], 50, 50);
	}

	//gtk_widget_set_name(button, "button");
	
	/* nastavenie zpracovania signalu clicked/pressed */
	for (int i = 0; i < 15; i++)
	{
		g_signal_connect(G_OBJECT(buttons[i]), "clicked", G_CALLBACK(move), NULL);
	}
	
	/* vložení tlačítka do tabulky */
	//gtk_container_add(GTK_CONTAINER(mainWindow), button);
	int k = 0;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (i != 3 || j != 3)
				gtk_table_attach_defaults (table, buttons[k++], j,j+1,i,i+1);
		}	
	}
	
	status = gtk_statusbar_new ();
	gtk_statusbar_push (GTK_STATUSBAR(status), 0, g_strdup_printf("Number of moves: %d", count));
	gtk_box_pack_start (GTK_BOX (box), status, FALSE, FALSE, 0);
		
	
	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(table),FALSE, FALSE, 0);
	
	/* realizacia objektov a zamapovanie na displej */
	gtk_widget_show_all(mainWindow);
	
	/* hlavna smycka */
	gtk_main();

	return 0;
}

