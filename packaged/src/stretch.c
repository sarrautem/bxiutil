/* -*- coding: utf-8 -*-
###############################################################################
# Author:  Quintin Jean-Noël <jean-noel.quintin@bull.net>
# Created on: 2015-02-26
# Contributors:
###############################################################################
# Copyright (C) 2015  Bull S. A. S.  -  All rights reserved
# Bull, Rue Jean Jaures, B.P.68, 78340, Les Clayes-sous-Bois
# This is not Free or Open Source software.
# Please contact Bull S. A. S. for details about its license.
###############################################################################
*/

#include "bxi/base/log.h"
#include "bxi/base/mem.h"
#include "bxi/util/stretch.h"

// *********************************************************************************
// ********************************** Defines **************************************
// *********************************************************************************


// *********************************************************************************
// ********************************** Types ****************************************
// *********************************************************************************
typedef struct bxistretch_s_t {
    size_t array_size; /**< size of the reallocable array which contain the chunk*/
    size_t chunk_nb;   /**< the number of chunk allocated*/
    size_t chunk_size; /**< the size of the chunks*/
    size_t element_nb; /**< number of allocated elements*/
    size_t element_size;
    char ** biarray;     /**< array containing the chunk*/
} bxistretch_s;

// *********************************************************************************
// ********************************** Static Functions  ****************************
// *********************************************************************************

// *********************************************************************************
// ********************************** Global Variables *****************************
// *********************************************************************************

SET_LOGGER(STRETCH_C_LOGGER, BXILOG_LIB_PREFIX "bxiutil.stretch");


// *********************************************************************************
// ********************************** Implementation           *********************
// *********************************************************************************

bxistretch_p bxistretch_new(size_t chunk_size,
                            size_t  element_size,
                            size_t element_nb) {

    bxistretch_p self = bximem_calloc(sizeof(*self));
    self->array_size = BXISTRETCH_ARRAY_SIZE;
    self->element_nb = 0;
    self->element_size = element_size;
    self->chunk_size = chunk_size;
    if (self->chunk_size == 0) self->chunk_size = BXISTRETCH_DEFAULT_CHUNK_SIZE;
    self->biarray = bximem_calloc(sizeof(*self->biarray) * BXISTRETCH_ARRAY_SIZE);
    self->chunk_nb = 0;
    if (element_nb > 1) bxistretch_hit(self, element_nb - 1);

    return self;
}

void bxistretch_destroy(bxistretch_p *self_p) {
    if (NULL == self_p) return;
    bxistretch_p self = *self_p;
    for (size_t i = 0; i < self->chunk_nb; i++) {
        BXIFREE(self->biarray[i]);
    }
    BXIFREE(self->biarray);
    BXIFREE(*self_p);
}

void * bxistretch_get(bxistretch_p self, size_t index) {
    size_t chunk_requested = ((index + self->chunk_size) / self->chunk_size) - 1;
    size_t position = index - chunk_requested * self->chunk_size;
    if (self->element_nb <= index) return NULL;
    return (void*)(uintptr_t)(self->biarray[chunk_requested] + position * self->element_size);
}

void * bxistretch_hit(bxistretch_p self, size_t index) {

    if (self->element_nb <= index) {

        size_t previously_chunk_nb = self->chunk_nb;
        size_t chunk_requested = (index + self->chunk_size) / self->chunk_size;
        self->chunk_nb = chunk_requested;

        if (self->array_size < self->chunk_nb) {
            size_t old_size = self->array_size;
            size_t new_size = (self->chunk_nb + old_size)/old_size * old_size;
            self->biarray = bximem_realloc(self->biarray, old_size, new_size);
            self->array_size = new_size;
        }

        for (size_t i = previously_chunk_nb; i < self->chunk_nb; i++) {
            self->biarray[i] = bximem_calloc(self->element_size * self->chunk_size);
        }
        self->element_nb = index + 1;
    }

    return bxistretch_get(self, index);
}


// *********************************************************************************
// ********************************** Static Functions Implementation  *************
// *********************************************************************************

