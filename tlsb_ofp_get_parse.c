/*
MIT License

Copyright (c) 2019 Holger Teutsch

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tlsb.h"
#include <errno.h>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

static int
get_element_text(char *xml, int start_ofs, int end_ofs, const char *tag, int *text_start, int *text_end)
{
    char stag[50], etag[50];
    sprintf(stag, "<%s>", tag);
    sprintf(etag, "</%s>", tag);

    char *s = strstr(xml + start_ofs, stag);
    if (NULL == s)
        return 0;

    s += strlen(stag);

    int c = xml[end_ofs];
    xml[end_ofs] = '\0';
    char *e = strstr(s, etag);
    xml[end_ofs] = c;
    if (NULL == e)
        return 0;

    *text_start = s - xml;
    *text_end = e - xml;
    return 1;
}

#define POSITION(tag) \
get_element_text(ofp, 0, ofp_len, tag, &out_s, &out_e)

#define EXTRACT(tag, field) \
do { \
    int s, e; \
    if (get_element_text(ofp, out_s, out_e, tag, &s, &e)) { \
        strncpy(ofp_info->field, ofp + s, MIN(sizeof(ofp_info->field), e - s)); \
    } \
} while (0)

int
tlsb_ofp_get_parse(const char *pilot_id, ofp_info_t *ofp_info)
{
    memset(ofp_info, 0, sizeof(*ofp_info));

    /* that should be plenty */
    int buflen = 1024 * 1014;
    char *ofp = malloc(buflen);
    if (NULL == ofp) {
        log_msg("Can't malloc");
        return 0;
    }

    int ofp_len;
#if 0
    int res = tlsb_ofp_get(pilot_id, ofp, buflen, &ofp_len);
#else
    FILE *f = fopen("xml.fetcher.xml", "r");
    int res = 0;
    if (f) {
        ofp_len = fread(ofp, 1, buflen, f);
        fclose(f);
        res = 1;
    } else {
        perror("fopen ");
    }
#endif
    if (0 == res) {
        return 0;
    }

    log_msg("got ofp %d bytes", ofp_len);
    //ofp[200] = 0;
    //log_msg(ofp);


    int fetch_s, fetch_e, status_s, status_e;
    if (get_element_text(ofp, 0, ofp_len, "fetch", &fetch_s, &fetch_e)
        && get_element_text(ofp, fetch_s, fetch_e, "status", &status_s, &status_e)) {
        if (strncmp(ofp + status_s, "Success", 7)) {
            strncpy(ofp_info->errmsg, ofp + status_s, MIN(sizeof(ofp_info->errmsg), status_e - status_s));
            goto out;
        }
    }

    int out_s, out_e;
    if (POSITION("aircraft")) {
        EXTRACT("icaocode", aircraft_icao);
        EXTRACT("max_passengers", max_passengers);
    }

    ofp_info->status = 1;

out:
    free(ofp);
    return 1;
}