/**
 * @file   module.c
 * @author António P. P. Almeida <appa@perusio.net>
 * @date   Wed Aug 17 12:06:52 2011
 *
 * @brief  A hello world module for Nginx.
 *
 * @section LICENSE
 *
 * Copyright (C) 2011 by Dominic Fallows, António P. P. Almeida <appa@perusio.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


/*
 * --------------------------------------------------------------------------
 *  Configuration interface to ngx
 *  -------------------------------------------------------------------------
 */
static char *ngx_http_hello_world(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

/**
 * This module provided directive: hello world.
 *
 */
static ngx_command_t commands[] = {
    { ngx_string("hello_world"),   /* directive */
      NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS, /* location context and takes no arguments*/
      ngx_http_hello_world,        /* configuration setup function */
      0,                           /* No offset. Only one context is supported. */
      0,                           /* No offset when storing the module configuration on struct. */
      NULL },

    ngx_null_command /* command termination */
};

/*
 * --------------------------------------------------------------------------
 *  Public interface to ngx
 * --------------------------------------------------------------------------
 */
/* The module context. */
static ngx_http_module_t module_ctx = {
    NULL,                          /* preconfiguration */
    NULL,                          /* postconfiguration */

    NULL,                          /* create main configuration */
    NULL,                          /* init main configuration */

    NULL,                          /* create server configuration */
    NULL,                          /* merge server configuration */

    NULL,                          /* create location configuration */
    NULL                           /* merge location configuration */
};

/* Module definition. */
ngx_module_t ngx_http_hello_world_module = {
    NGX_MODULE_V1,
    &module_ctx,                   /* module context */
    commands,                      /* module directives */
    NGX_HTTP_MODULE,               /* module type */
    NULL,                          /* init master */
    NULL,                          /* init module */
    NULL,                          /* init process */
    NULL,                          /* init thread */
    NULL,                          /* exit thread */
    NULL,                          /* exit process */
    NULL,                          /* exit master */
    NGX_MODULE_V1_PADDING
};

/*
 * --------------------------------------------------------------------------
 *  Private implementation
 * --------------------------------------------------------------------------
 */
static ngx_int_t handler(ngx_http_request_t *r);

/* The hello world string. */
static u_char ngx_hello_world[] = "hello world";
static size_t ngx_hello_world_len = sizeof(ngx_hello_world) - 1;

/**
 * Configuration setup function that installs the content handler.
 *
 * @param cf
 *   Module configuration structure pointer.
 * @param cmd
 *   Module directives structure pointer.
 * @param conf
 *   Module configuration structure pointer.
 * @return string
 *   Status of the configuration setup.
 */
static char *ngx_http_hello_world(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    /* pointer to core location configuration */
    /* Install the hello world handler. */
    ngx_http_core_loc_conf_t *clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = handler;

    return NGX_CONF_OK;
} /* ngx_http_hello_world */

/**
 * Content handler.
 *
 * @param r
 *   Pointer to the request structure. See http_request.h.
 * @return
 *   The status of the response generation.
 */
static ngx_int_t handler(ngx_http_request_t *r)
{
    /* Set the Content-Type header. */
    static const char mime_type[] = "text/plain";
    r->headers_out.content_type.len = sizeof(mime_type) - 1;
    r->headers_out.content_type.data = (u_char *) mime_type;

    /* Sending the headers for the reply. */
    r->headers_out.status = NGX_HTTP_OK; /* 200 status code */
    r->headers_out.content_length_n = ngx_hello_world_len; /* Get the content length of the body. */
    ngx_http_send_header(r);       /* Send the headers */

    /* Allocate a new buffer for sending out the reply. */
    ngx_buf_t *b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
    b->pos = ngx_hello_world;      /* first position in memory of the data */
    b->last = ngx_hello_world + ngx_hello_world_len; /* last position in memory of the data */
    b->memory = 1;                 /* content is in read-only memory */
    b->last_buf = 1;               /* there will be no more buffers in the request */
    ngx_chain_t out = { b, NULL }; /* Insertion in the buffer chain -- just one buffer */

    /* Send the body, and return the status code of the output filter chain. */
    return ngx_http_output_filter(r, &out);
} /* handler */
