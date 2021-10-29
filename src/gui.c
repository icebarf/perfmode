#include "gui.h"
#include "perfmode.h"

void activate(GtkApplication* app, gpointer user_data)
{
    GtkWidget* window;
    GtkWidget* grid;
    GtkWidget* button;

    /* create a new window */
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Window");

    /* container for button packing */
    grid = gtk_grid_new();

    /* pack container in a window */
    gtk_window_set_child(GTK_WINDOW(window), grid);

    /* Heading button - does nothing */
    button = gtk_button_new_with_label("Keyboard Backlight");
    gtk_grid_attach(GTK_GRID(grid), button, 0, 0, 2, 1);

    /* LED OFF */
    button = gtk_button_new_with_label("Off");
    g_signal_connect(button, "clicked", G_CALLBACK(led_off), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 1, 1, 1);

    /* LED MIN */
    button = gtk_button_new_with_label("Min");
    g_signal_connect(button, "clicked", G_CALLBACK(led_min), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 1, 1, 1, 1);

    /* LED MED */
    button = gtk_button_new_with_label("Med");
    g_signal_connect(button, "clicked", G_CALLBACK(led_med), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 2, 1, 1);

    /* LED MAX */
    button = gtk_button_new_with_label("Max");
    g_signal_connect(button, "clicked", G_CALLBACK(led_max), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 1, 2, 1, 1);

    /* Heading button - does nothing */
    button = gtk_button_new_with_label("Keyboard Backlight");
    gtk_grid_attach(GTK_GRID(grid), button, 4, 0, 2, 1);

    /* SILENT MODE */
    button = gtk_button_new_with_label("Silent");
    g_signal_connect(button, "clicked", G_CALLBACK(fan_silent), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 4, 1, 1, 1);

    /* TURBO MODE */
    button = gtk_button_new_with_label("Balanced");
    g_signal_connect(button, "clicked", G_CALLBACK(fan_balanced), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 5, 1, 1, 1);

    /* TURBO MODE */
    button = gtk_button_new_with_label("Turbo");
    g_signal_connect(button, "clicked", G_CALLBACK(fan_turbo), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 4, 2, 2, 1);

    gtk_widget_show(window);
}