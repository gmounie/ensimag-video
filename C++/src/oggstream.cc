#include "ensitheora.hpp"
#include "ensivorbis.hpp"
#include "stream_common.hpp"
#include "synchro.hpp"
#include <cassert>
#include <fstream>
#include <ogg/ogg.h>
#include <stdio.h>
#include <string>

using namespace std;

ogg_sync_state oggtheorastate,
    oggvorbisstate; /* sync and verify incoming physical bitstream */
ogg_page theorapage,
    vorbispage; /* one Ogg bitstream page. Vorbis packets are inside */

void theoraStreamReader(const string filename) {
  // char *filename = (char *) arg;

  ifstream vf(filename);
  // FILE *vf = fopen(filename, "r");
  assert(vf);

  // Initialisation de l'automate OGG
  int res = ogg_sync_init(&oggtheorastate);
  assert(res == 0);

  int respac = 0;
  struct streamstate *s;

  while (!fini) {
    // printf("theora loop\n");// vérifier si le fichier ne serait pas fini
    if (vf.eof()) {
      fini = true;
      return;
    }

    if (respac == 0) {
      pageReader(vf, &oggtheorastate, &theorapage);
      s = getStreamState(&oggtheorastate, &theorapage, streamtype::TYPE_THEORA);

      // ignorer le stream vorbis
      if (s->strtype == streamtype::TYPE_VORBIS)
        continue;

      respac = addPageGetPacket(&theorapage, s);
    } else {
      respac = getPacket(s);
    }
    switch (respac) {
    case -1:
      s->nbpacketoutsync++;
      printf("out of sync: gap in data\n");
      break;
    case 0:
      // more pages (data) are needed to build a full packet
      continue;
      break;
    case 1:
      s->nbpacket++;
      break;
    }

    if (decodeAllHeaders(respac, s, streamtype::TYPE_THEORA))
      continue;

    if (s->strtype == streamtype::TYPE_THEORA && s->headersRead) {
      theora2SDL(s);
    }
  }
  vf.close();
}

void vorbisStreamReader(const string filename) {
  // char *filename = (char *) arg;
  ifstream vf(filename);
  // FILE *vf = fopen(filename, "r");
  assert(vf);

  // Initialisation de l'automate OGG
  int res = ogg_sync_init(&oggvorbisstate);
  assert(res == 0);

  int respac = 0;
  struct streamstate *s;

  while (!fini) {
    // printf ("vorbis loop \n");
    // vérifier si le fichier ne serait pas fini
    if (vf.eof()) {
      fini = true;
      printf("FIN de la lecture de VORBIS !");
      break;
    }

    if (respac == 0) {
      pageReader(vf, &oggvorbisstate, &vorbispage);
      s = getStreamState(&oggvorbisstate, &vorbispage, streamtype::TYPE_VORBIS);

      // ignorer le stream theora
      if (s->strtype == streamtype::TYPE_THEORA)
        continue;

      // ajouter la page dans le décodeur et tenter d'extraire un
      // packet
      respac = addPageGetPacket(&vorbispage, s);
    } else {
      respac = getPacket(s);
    }

    switch (respac) {
    case -1:
      s->nbpacketoutsync++;
      printf("out of sync: gap in data\n");
      break;
    case 0:
      // more pages (data) are needed to build a full packet
      continue;
      break;
    case 1:
      s->nbpacket++;
      break;
    }

    if (decodeAllHeaders(respac, s, streamtype::TYPE_THEORA))
      continue;

    // boucle principale de lecture vorbis
    if (s->strtype == streamtype::TYPE_VORBIS && s->headersRead) {
      vorbis2SDL(s);
    }
  }
  vf.close();
}
