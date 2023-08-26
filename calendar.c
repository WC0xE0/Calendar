#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "calendar.h"

/* Initializes a calendar structure based on provided parameters. Allocates 
 * memory for the Calendar structure, the name field, and the events field. 
 * The out parameter calendar provides access to the new Calendar structure. */
int init_calendar(const char *name, int days,
                  int (*comp_func) (const void *ptr1, const void *ptr2),
                  void (*free_info_func) (void *ptr), Calendar **calendar) {
   if (calendar == NULL || name == NULL || days < 1) {
      return FAILURE;
   }

   *calendar = malloc(sizeof(Calendar)); /* 1 */
   if (*calendar == NULL) { 
      return FAILURE;
   }
    
   (*calendar)->name = malloc(strlen(name) + 1); /* 2 */ 
   if ((*calendar)->name == NULL) { 
      free(*calendar);
      return FAILURE;
   }
   strcpy((*calendar)->name, name);

   /* The events field is an array of pointers to event structures. */
   (*calendar)->events = calloc(days, sizeof(Event *)); /* 3 */
   if ((*calendar)->events == NULL) { 
      free((*calendar)->name);
      free(*calendar);
      return FAILURE;
   }

   (*calendar)->total_events = 0; 
   (*calendar)->days = days; 
   (*calendar)->free_info_func = free_info_func;
   (*calendar)->comp_func = comp_func;

   return SUCCESS;                     
}

/* Prints the calendar's name, days, and total number of events if print_all
 * is true; otherwise this info will not be printed. Info about each event 
 * (name, start time, duration) will be printed regardless of the value of 
 * print_all. The heading "**** events ****" will always be printed. */
int print_calendar(Calendar *calendar, FILE *output_stream, int print_all) {
   int i;
   Event *day_head, *curr;

   if (calendar == NULL || output_stream == NULL) {
      return FAILURE;
   }

   if (print_all) {
      fprintf(output_stream, "Calendar's Name: \"%s\"\n", calendar->name);
      fprintf(output_stream, "Days: %d\n", calendar->days);
      fprintf(output_stream, "Total Events: %d\n\n", calendar->total_events);
   }

   fprintf(output_stream, "**** Events ****\n");
   if (calendar->total_events == 0) {
      return SUCCESS;
   }

   for (i = 0; i < calendar->days; i++) {
      fprintf(output_stream, "Day %d\n", i + 1);

      if (calendar->events[i] != NULL) {
         day_head = calendar->events[i]; 
         curr = day_head;

         while (curr != NULL) {      
            fprintf(output_stream, "Event's Name: \"%s\", ", curr->name);
            fprintf(output_stream, "Start_time: %d, ", curr->start_time);
            fprintf(output_stream, "Duration: %d\n", curr-> duration_minutes);
            curr = curr->next; 
         }
      }
   }
   return SUCCESS;
}

/* Adds the specified event to the list associated with the day parameter.
 * The event must be added in increasing sorted order using the comparison
 * function (comp_func) that compares two events. 
 * The function allocates memory for the new event and for its name. */
int add_event(Calendar *calendar, const char *name, int start_time,
              int duration_minutes, void *info, int day) {
   Event *event, *prev, *curr, **out_event = NULL;

   if (!calendar || !name || start_time < 0 || start_time > 2400 || 
       duration_minutes <= 0 || day < 1 || day > calendar->days) {
      return FAILURE;
   }

   /* Check if event already exists in Calendar */
   if (find_event(calendar, name, out_event) == SUCCESS) {
      return FAILURE;
   }

   /* Returns FAILURE if any memory allocation fails */
   event = malloc(sizeof(Event));
   if (event == NULL) {
      return FAILURE;
   }
   
   event->name = malloc(strlen(name) + 1); 
   if (event->name == NULL) { 
      free(event);
      return FAILURE;
   }
   strcpy(event->name, name); 

   /* Other fields of the event are initialized based on parameter values */
   event->info = info; 
   event->start_time = start_time; 
   event->duration_minutes = duration_minutes;
   event->next = NULL;

   /* After event has been created, decide where to insert it in the list */
   prev = NULL, curr = calendar->events[day - 1];

   while (curr != NULL && calendar->comp_func(event, curr) > 0) {
      prev = curr;
      curr = curr->next;
   } 

   if (prev == NULL) { /* then insert at beginning */
      event->next = curr;
      calendar->events[day - 1] = event;
   } else {
      event->next = curr;
      prev->next = event;
   }

   calendar->total_events++;

   return SUCCESS;
}

/* Returns a pointer (via the out parameter 'event') to the event with the
 * specified name (if any). If the event parameter is NULL, no pointer will be
 * returned. Do not modify the out parameter 'event' unless the event is found. */
int find_event(Calendar *calendar, const char *name, Event **event) {
   int i;
   Event *curr_event;

   if (calendar == NULL || name == NULL) {
      return FAILURE;
   }

   for (i = 0; i < calendar->days; i++) {
      curr_event = calendar->events[i];
      while (curr_event != NULL) {
         if (strcmp(curr_event->name, name) == 0) {
            if (event != NULL) { /* the out parameter */
               *event = curr_event;
            }
            return SUCCESS;
         }
         curr_event = curr_event->next;
      }
   }

   return FAILURE;
}

/* Returns a pointer (via the out parameter 'event') to the event with the 
 * specified name in the specified day (if such event exist). If the event 
 * parameter is NULL, no pointer will be returned. Do not modify the out 
 * parameter 'event' unless the event is found. */
int find_event_in_day(Calendar *calendar, const char *name, int day,
                      Event **event) {
   Event *curr_event;

   if (calendar == NULL || name == NULL || day < 1 || day > calendar->days) {
      return FAILURE;
   }

   curr_event = calendar->events[day - 1];
   while (curr_event != NULL) {
      if (strcmp(curr_event->name, name) == 0) {
         if (event != NULL) { /* the out parameter */
            *event = curr_event;
         }
         return SUCCESS;
      }
      curr_event = curr_event->next;
   }

   return FAILURE;
}

/* Returns the info pointer associated with the specified event. 
 * Returns NULL if the event is not found. 
 * You can assume the name and calendar parameters are not NULL. */
void *get_event_info(Calendar *calendar, const char *name) {
   void *info = NULL;
   Event *event, **out_event = NULL;
   int i;

   if (find_event(calendar, name, out_event) == SUCCESS) {
      for (i = 0; i < calendar->days; i++) {
         event = calendar->events[i];
         while (event != NULL) {
            if (strcmp(event->name, name) == 0) {
               info = event->info;

               return info;
            }
            event = event->next;
         }
      }
   }

   return NULL;
}

/* Remove specified event from calendar; freeing any memory allocated for it */
int remove_event(Calendar *calendar, const char *name) {
   Event *curr_event, *prev_event, **out_event = NULL;
   int day = 1;

   if (calendar == NULL || name == NULL) {
      return FAILURE;
   }

   while (day <= calendar->days) {
      if (find_event_in_day(calendar, name, day, out_event) == SUCCESS) {
         curr_event = calendar->events[day - 1];
         prev_event = NULL;

         while (strcmp(curr_event->name, name) != 0) {
            prev_event = curr_event;
            curr_event = curr_event->next; 
         }

         /* found event, now need to delete it from linked list */
         if (prev_event == NULL) {
            calendar->events[day - 1] = curr_event->next;
         } else {
            prev_event->next = curr_event->next;
         }

         /* free the memories of curr_event */
         if (curr_event->info != NULL && calendar->free_info_func != NULL) {
            calendar->free_info_func(curr_event->info);
         }
         free(curr_event->name);
         free(curr_event);

         calendar->total_events--; 

         return SUCCESS;
      }         
      day++;
   }

   /* If the event by the specified name cannot be found, return FAILURE */
   return FAILURE;
}

/* Removes all the event lists in the calendar; set them to empty lists */
int clear_calendar(Calendar *calendar) {
   Event *prev, *curr;
   int i;

   if (calendar == NULL) {
      return FAILURE;
   }

   for (i = 0; i < calendar->days; i++) {
      prev = NULL;
      curr = calendar->events[i];

      while (curr != NULL) {
         prev = curr;
         curr = curr->next;
         remove_event(calendar, prev->name);
      }
      calendar->events[i] = NULL;
   }
   calendar->total_events = 0;

   return SUCCESS;
}

/* Removes all events for the specified day, setting its event list to empty */ 
int clear_day(Calendar *calendar, int day) {
   Event *curr, *prev;

   if (calendar == NULL || day < 1 || day > calendar->days) {
      return FAILURE;
   }

   curr = calendar->events[day - 1]; 
   prev = NULL;

   while (curr != NULL) {
      prev = curr;
      curr = curr->next;
      remove_event(calendar, prev->name); 
      /* If an event has an info field other than NULL and a free_info_func 
       * exists, the function will be called on the info field. 
       * The remove function already takes care of this */
   }
   calendar->events[day - 1] = NULL;

   return SUCCESS;
}

/* Releases momery that was dynamically-allocated for the calendar */
int destroy_calendar(Calendar *calendar) {
   if (calendar == NULL) {
      return FAILURE;
   }

   clear_calendar(calendar);
   free(calendar->name);
   free(calendar->events);
   free(calendar);

   return SUCCESS;
}

