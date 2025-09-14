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
  char *theme;
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
  sc->theme = malloc(__max_name_len * sizeof(char));
  if (sc->theme == NULL) {
    free(sc->name);
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
  if (sc->theme != NULL)
    free(sc->theme);
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
    } else if (strncmp(line, "theme=", 6) == 0) {
      strncpy(sc->theme, line + 6, __max_name_len - 1);
      sc->theme[__max_name_len - 1] = '\0';
      sc->theme[strcspn(sc->theme, "\n")] = '\0';
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
              "6.5.1/css/all.min.css\">\n");
  fprintf(fp, "  <link rel=\"stylesheet\" href=\"themes/%s.css\">\n",
          sc->theme ? sc->theme : "dark");
  fprintf(fp, "</head>\n");
  fprintf(fp, "<body>\n");
  fprintf(fp, "  <main class=\"container\">\n");
  fprintf(fp, "    <h1>Hey, I'm %s 👋</h1>\n",
          sc->name ? sc->name : "Your Name");

  if (sc->social_count > 0) {
    fprintf(fp, "    <ul class=\"socials-list\">\n");
    for (int i = 0; i < sc->social_count; i++) {
      const char *icon_class = "";
      const char *base_url = "";
      const char *social_name = "";
      switch (sc->socials[i].type) {
      case X:
        icon_class = "fa-brands fa-twitter";
        base_url = "https://x.com/";
        social_name = "X";
        break;
      case GITHUB:
        icon_class = "fa-brands fa-github";
        base_url = "https://github.com/";
        social_name = "GitHub";
        break;
      case LINKEDIN:
        icon_class = "fa-brands fa-linkedin";
        base_url = "https://linkedin.com/in/";
        social_name = "LinkedIn";
        break;
      }
      fprintf(fp,
              "      <li><a href=\"%s%s\" title=\"%s\" target=\"_blank\" "
              "rel=\"noopener noreferrer\"><i class=\"%s\"></i></a></li>\n",
              base_url, sc->socials[i].username, social_name, icon_class);
    }
    fprintf(fp, "    </ul>\n");
  }
  fprintf(fp, "  </main>\n");
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
