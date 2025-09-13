#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define __max_socials 4
#define __max_name_len 24
typedef enum SocialType {
  X,
  GITHUB,
  LINKEDIN,
} SocialType;
typedef struct Socials {
  SocialType type;
  char *username;
} Socials;
typedef struct SiteConfig {
  char *name;
  Socials socials[__max_socials];
  int social_count;
} SiteConfig;
SiteConfig *init_SiteConfig() {
  SiteConfig *sc = malloc(sizeof(SiteConfig));
  if (sc == NULL) {
    return NULL;
  }
  sc->name = malloc(__max_name_len * sizeof(char));
  if (sc->name == NULL) {
    free(sc);
    return NULL;
  }
  sc->social_count = 0;
  for (int i = 0; i < __max_socials; i++) {
    sc->socials[i].username = NULL;
  }
  return sc;
}
void free_SiteConfig(SiteConfig *sc) {
  if (sc == NULL)
    return;
  if (sc->name != NULL)
    free(sc->name);
  for (int i = 0; i < sc->social_count; i++) {
    if (sc->socials[i].username != NULL)
      free(sc->socials[i].username);
  }
  free(sc);
}
void add_social(SiteConfig *sc, SocialType type, const char *username_start) {
  if (sc->social_count < __max_socials) {
    sc->socials[sc->social_count].type = type;
    sc->socials[sc->social_count].username = malloc(strlen(username_start) + 1);
    if (sc->socials[sc->social_count].username != NULL) {
      strcpy(sc->socials[sc->social_count].username, username_start);
      sc->socials[sc->social_count]
          .username[strcspn(sc->socials[sc->social_count].username, "\n")] =
          '\0';
      sc->social_count++;
    }
  }
}
void parse_config(SiteConfig *sc) {
  FILE *fp = fopen("config.txt", "r");
  if (fp == NULL) {
    fprintf(stderr, "could not open config\n");
    return;
  }
  char line[256];
  while (fgets(line, sizeof(line), fp)) {
    if (strncmp(line, "name=", 5) == 0) {
      strncpy(sc->name, line + 5, __max_name_len - 1);
      sc->name[__max_name_len - 1] = '\0';
      sc->name[strcspn(sc->name, "\n")] = '\0';
    } else if (strncmp(line, "twitter=", 8) == 0) {
      add_social(sc, X, line + 8);
    } else if (strncmp(line, "github=", 7) == 0) {
      add_social(sc, GITHUB, line + 7);
    } else if (strncmp(line, "linkedin=", 9) == 0) {
      add_social(sc, LINKEDIN, line + 9);
    } else if (strncmp(line, "name=", 5) == 0) {
      strncpy(sc->name, line + 5, __max_name_len - 1);
      sc->name[__max_name_len - 1] = '\0';
      sc->name[strcspn(sc->name, "\n")] = '\0';
    }
  }
  fclose(fp);
}
void generate_html(SiteConfig *sc) {
  FILE *fp = fopen("index.html", "w");
  if (fp == NULL) {
    fprintf(stderr, "could not create index.html\n");
    return;
  }
  fprintf(fp, "<!DOCTYPE html>\n");
  fprintf(fp, "<html lang=\"en\">\n");
  fprintf(fp, "<head>\n");
  fprintf(fp, "  <meta charset=\"UTF-8\">\n");
  fprintf(fp, "  <meta name=\"viewport\" content=\"width=device-width, "
              "initial-scale=1.0\">\n");
  fprintf(fp, "  <title>%s</title>\n", sc->name ? sc->name : "My Site");
  fprintf(fp, "  <link rel=\"stylesheet\" "
              "href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/"
              "6.0.0/css/all.min.css\">\n");
  fprintf(fp, "</head>\n");
  fprintf(fp, "<body>\n");
  fprintf(fp, "Hello %s\n,", sc->name);
  for (int i = 0; i < sc->social_count; i++) {
    switch (sc->socials[i].type) {
    case X:
      fprintf(fp,
              "  <p><a href=\"https://x.com/%s\"><i class=\"fa-brands "
              "fa-twitter\"></i> @%s</a></p>\n",
              sc->socials[i].username, sc->socials[i].username);
      break;
    case GITHUB:
      fprintf(fp,
              "  <p><a href=\"https://github.com/%s\"><i class=\"fab "
              "fa-github\"></i> %s</a></p>\n",
              sc->socials[i].username, sc->socials[i].username);
      break;
    case LINKEDIN:
      fprintf(fp,
              "  <p><a href=\"https://linkedin.com/in/%s\"><i class=\"fab "
              "fa-linkedin\"></i> %s</a></p>\n",
              sc->socials[i].username, sc->socials[i].username);
      break;
    }
  }
  fprintf(fp, "</body>\n");
  fprintf(fp, "</html>\n");
  fclose(fp);
}
int main() {
  SiteConfig *sc = init_SiteConfig();
  if (sc == NULL) {
    fprintf(stderr, "Failed to initialize SiteConfig\n");
    return 1;
  }
  parse_config(sc);
  generate_html(sc);
  free_SiteConfig(sc);
  return 0;
}
