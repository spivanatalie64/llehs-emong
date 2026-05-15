/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/*
 * st-private.h: Private declarations and functions
 *
 * Copyright 2009, 2010 Red Hat, Inc.
 * Copyright 2010 Florian Müllner
 * Copyright 2010 Intel Corporation
 * Copyright 2010 Giovanni Campagna
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <math.h>
#include <string.h>

#include "st-private.h"
#include "st-image-content.h"

/**
 * _st_actor_get_preferred_width:
 * @actor: a #ClutterActor
 * @for_height: as with clutter_actor_get_preferred_width()
 * @y_fill: %TRUE if @actor will fill its allocation vertically
 * @min_width_p: as with clutter_actor_get_preferred_width()
 * @natural_width_p: as with clutter_actor_get_preferred_width()
 *
 * Like clutter_actor_get_preferred_width(), but if @y_fill is %FALSE,
 * then it will compute a width request based on the assumption that
 * @actor will be given an allocation no taller than its natural
 * height.
 */
void
_st_actor_get_preferred_width  (ClutterActor *actor,
                                 gfloat        for_height,
                                 gboolean      y_fill,
                                 gfloat       *min_width_p,
                                 gfloat       *natural_width_p)
{
  if (!y_fill && for_height != -1)
    {
      ClutterRequestMode mode;
      gfloat natural_height;

      mode = clutter_actor_get_request_mode (actor);
      if (mode == CLUTTER_REQUEST_WIDTH_FOR_HEIGHT)
        {
          clutter_actor_get_preferred_height (actor, -1, NULL, &natural_height);
          if (for_height > natural_height)
            for_height = natural_height;
        }
    }

  clutter_actor_get_preferred_width (actor, for_height, min_width_p, natural_width_p);
}

/**
 * _st_actor_get_preferred_height:
 * @actor: a #ClutterActor
 * @for_width: as with clutter_actor_get_preferred_height()
 * @x_fill: %TRUE if @actor will fill its allocation horizontally
 * @min_height_p: as with clutter_actor_get_preferred_height()
 * @natural_height_p: as with clutter_actor_get_preferred_height()
 *
 * Like clutter_actor_get_preferred_height(), but if @x_fill is
 * %FALSE, then it will compute a height request based on the
 * assumption that @actor will be given an allocation no wider than
 * its natural width.
 */
void
_st_actor_get_preferred_height (ClutterActor *actor,
                                 gfloat        for_width,
                                 gboolean      x_fill,
                                 gfloat       *min_height_p,
                                 gfloat       *natural_height_p)
{
  if (!x_fill && for_width != -1)
    {
      ClutterRequestMode mode;
      gfloat natural_width;

      mode = clutter_actor_get_request_mode (actor);
      if (mode == CLUTTER_REQUEST_HEIGHT_FOR_WIDTH)
        {
          clutter_actor_get_preferred_width (actor, -1, NULL, &natural_width);
          if (for_width > natural_width)
            for_width = natural_width;
        }
    }

  clutter_actor_get_preferred_height (actor, for_width, min_height_p, natural_height_p);
}

/**
 * _st_set_text_from_style:
 * @text: Target #ClutterText
 * @theme_node: Source #StThemeNode
 *
 * Set various GObject properties of the @text object using
 * CSS information from @theme_node.
 */
void
_st_set_text_from_style (ClutterText *text,
                         StThemeNode *theme_node)
{

  ClutterColor color;
  StTextDecoration decoration;
  PangoAttrList *attribs = NULL;
  const PangoFontDescription *font;
  PangoAttribute *foreground;
  StTextAlign align;
  gdouble spacing;
  gchar *font_features;

  font = st_theme_node_get_font (theme_node);
  clutter_text_set_font_description (text, (PangoFontDescription *) font);

  attribs = pango_attr_list_new ();

  st_theme_node_get_foreground_color (theme_node, (CoglColor *)&color);
  clutter_text_set_cursor_color (text, &color);
  foreground = pango_attr_foreground_new (clutter_color_get_red (&color) * 255,
                                          clutter_color_get_green (&color) * 255,
                                          clutter_color_get_blue (&color) * 255);
  pango_attr_list_insert (attribs, foreground);

  if (clutter_color_get_alpha (&color) != 255)
    {
      PangoAttribute *alpha;

      /* An alpha value of 0 means "system inherited", so the
       * minimum regular value is 1.
       */
      if (clutter_color_get_alpha (&color) == 0)
        alpha = pango_attr_foreground_alpha_new (1);
      else
        alpha = pango_attr_foreground_alpha_new (clutter_color_get_alpha (&color) * 255);

      pango_attr_list_insert (attribs, alpha);
    }

  decoration = st_theme_node_get_text_decoration (theme_node);
  if (decoration)
    {
      if (decoration & ST_TEXT_DECORATION_UNDERLINE)
        {
          PangoAttribute *underline = pango_attr_underline_new (PANGO_UNDERLINE_SINGLE);
          pango_attr_list_insert (attribs, underline);
        }
      if (decoration & ST_TEXT_DECORATION_LINE_THROUGH)
        {
          PangoAttribute *strikethrough = pango_attr_strikethrough_new (TRUE);
          pango_attr_list_insert (attribs, strikethrough);
        }
      /* Pango doesn't have an equivalent attribute for _OVERLINE, and we deliberately
       * skip BLINK (for now...)
       */
    }

  spacing = st_theme_node_get_letter_spacing (theme_node);
  if (spacing)
    {
      PangoAttribute *letter_spacing = pango_attr_letter_spacing_new ((int)(.5 + spacing) * PANGO_SCALE);
      pango_attr_list_insert (attribs, letter_spacing);
    }

  font_features = st_theme_node_get_font_features (theme_node);
  if (font_features)
    {
      pango_attr_list_insert (attribs, pango_attr_font_features_new (font_features));
      g_free (font_features);
    }

  clutter_text_set_attributes (text, attribs);

  if (attribs)
    pango_attr_list_unref (attribs);

  align = st_theme_node_get_text_align (theme_node);
  if (align == ST_TEXT_ALIGN_JUSTIFY)
    {
      clutter_text_set_justify (text, TRUE);
      clutter_text_set_line_alignment (text, PANGO_ALIGN_LEFT);
    }
  else
    {
      clutter_text_set_justify (text, FALSE);
      clutter_text_set_line_alignment (text, (PangoAlignment) align);
    }
}

/**
 * _st_create_texture_pipeline:
 * @src_texture: The CoglTexture for the pipeline
 *
 * Creates a simple pipeline which contains the given texture as a
 * single layer.
 */
CoglPipeline *
_st_create_texture_pipeline (CoglTexture *src_texture)
{
  static CoglPipeline *texture_pipeline_template = NULL;
  CoglPipeline *pipeline;

  g_return_val_if_fail (src_texture != NULL, NULL);

  /* The only state used in the pipeline that would affect the shader
   * generation is the texture type on the layer. Therefore we create
   * a template pipeline which sets this state and all texture
   * pipelines are created as a copy of this. That way Cogl can find
   * the shader state for the pipeline more quickly by looking at the
   * pipeline ancestry instead of resorting to the shader cache. */
  if (G_UNLIKELY (texture_pipeline_template == NULL))
    {
      /* cogl_texture_get_context removed in newer Cogl versions
       * Using cogl_get_context() as replacement */
      CoglContext *ctx = cogl_get_context ();
      if (!ctx) {
        /* Fallback: try to get context from display */
        CoglDisplay *display = cogl_display_get_default ();
        if (display)
          ctx = cogl_display_get_context (display);
      }
      
      if (ctx)
        texture_pipeline_template = cogl_pipeline_new (ctx);
      else
        texture_pipeline_template = NULL; /* Handle error case */
      if (texture_pipeline_template) /* Add check for NULL pipeline before calling method */
        cogl_pipeline_set_layer_null_texture (texture_pipeline_template, 0);
    }

  pipeline = cogl_pipeline_copy (texture_pipeline_template);

  if (src_texture != NULL)
    cogl_pipeline_set_layer_texture (pipeline, 0, src_texture);

  return pipeline;
}

/*****
 * Shadows
 *****/

static gdouble *
calculate_gaussian_kernel (gdouble   sigma,
                           guint     n_values)
{
  gdouble *ret, sum;
  gdouble exp_divisor;
  int half, i;

  g_return_val_if_fail (sigma > 0, NULL);

  half = n_values / 2;

  ret = g_malloc (n_values * sizeof (gdouble));
  sum = 0.0;

  exp_divisor = 2 * sigma * sigma;

  /* n_values of 1D Gauss function */
  for (i = 0; i < (int)n_values; i++)
    {
      ret[i] = exp (-(i - half) * (i - half) / exp_divisor);
      sum += ret[i];
    }

  /* normalize */
  for (i = 0; i < (int)n_values; i++)
    ret[i] /= sum;

  return ret;
}

static guchar *
blur_pixels (guchar  *pixels_in,
             gint     width_in,
             gint     height_in,
             gint     rowstride_in,
             gdouble  blur,
             gint    *width_out,
             gint    *height_out,
             size_t  *rowstride_out)
{
  guchar *pixels_out;
  gdouble sigma;

  /* The CSS specification defines (or will define) the blur radius as twice
   * the Gaussian standard deviation. See:
   *
   * http://lists.w3.org/Archives/Public/www-style/2010Sep/0002.html
   */
  sigma = blur / 2.;

  if ((guint) blur == 0)
    {
      *width_out  = width_in;
      *height_out = height_in;
      *rowstride_out = rowstride_in;
      pixels_out = g_memdup2 (pixels_in, *rowstride_out * *height_out);
    }
  else
    {
      gdouble *kernel;
      guchar  *line;
      gint     n_values, half;
      gint     x_in, y_in, x_out, y_out, i;

      n_values = (gint) 5 * sigma;
      half = n_values / 2;

      *width_out  = width_in  + 2 * half;
      *height_out = height_in + 2 * half;
      *rowstride_out = (*width_out + 3) & ~3;

      pixels_out = g_malloc0 (*rowstride_out * *height_out);
      line       = g_malloc0 (*rowstride_out);

      kernel = calculate_gaussian_kernel (sigma, n_values);

      /* vertical blur */
      for (x_in = 0; x_in < width_in; x_in++)
        for (y_out = 0; y_out < *height_out; y_out++)
          {
            guchar *pixel_in, *pixel_out;
            gint i0, i1;

            y_in = y_out - half;

            /* We read from the source at 'y = y_in + i - half'; clamp the
             * full i range [0, n_values) so that y is in [0, height_in).
             */
            i0 = MAX (half - y_in, 0);
            i1 = MIN (height_in + half - y_in, n_values);

            pixel_in  =  pixels_in + (y_in + i0 - half) * rowstride_in + x_in;
            pixel_out =  pixels_out + y_out * *rowstride_out + (x_in + half);

            for (i = i0; i < i1; i++)
              {
                *pixel_out += *pixel_in * kernel[i];
                pixel_in += rowstride_in;
              }
          }

      /* horizontal blur */
      for (y_out = 0; y_out < *height_out; y_out++)
        {
          memcpy (line, pixels_out + y_out * *rowstride_out, *rowstride_out);

          for (x_out = 0; x_out < *width_out; x_out++)
            {
              gint i0, i1;
              guchar *pixel_out, *pixel_in;

              /* We read from the source at 'x = x_out + i - half'; clamp the
               * full i range [0, n_values) so that x is in [0, width_out).
               */
              i0 = MAX (half - x_out, 0);
              i1 = MIN (*width_out + half - x_out, n_values);

              pixel_in  = line + x_out + i0 - half;
              pixel_out = pixels_out + *rowstride_out * y_out + x_out;

              *pixel_out = 0;
              for (i = i0; i < i1; i++)
                {
                  *pixel_out += *pixel_in * kernel[i];
                  pixel_in++;
                }
            }
        }
      g_free (kernel);
      g_free (line);
    }

  return pixels_out;
}

CoglPipeline *
_st_create_shadow_pipeline (StShadow            *shadow_spec,
                            ClutterPaintContext *paint_context,
                            CoglTexture         *src_texture,
                            float                resource_scale)
{
  g_autoptr (ClutterPaintNode) texture_node = NULL;
  g_autoptr (ClutterPaintNode) blur_node = NULL;
  g_autoptr (CoglOffscreen) offscreen = NULL;
  g_autoptr (GError) error = NULL;
  CoglContext *cogl_context;
  ClutterPaintContext *nested_paint_context;
  ClutterColorState *color_state;
  CoglFramebuffer *fb;
  CoglPipeline *pipeline;
  CoglTexture *texture;
  float sampling_radius;
  float radius;
  int src_height, dst_height;
  int src_width, dst_width;
  CoglPipeline *texture_pipeline;

  static CoglPipelineKey texture_pipeline_key =
    "st-create-shadow-pipeline-saturate-alpha";
  static CoglPipeline *shadow_pipeline_template = NULL;

  g_return_val_if_fail (shadow_spec != NULL, NULL);
  g_return_val_if_fail (src_texture != NULL, NULL);

  radius = resource_scale * shadow_spec->blur;
  sampling_radius = ceilf (radius);

  src_width = cogl_texture_get_width (src_texture);
  src_height = cogl_texture_get_height (src_texture);
  cogl_context = cogl_texture_get_context (src_texture);
  dst_width = src_width + 2 * sampling_radius;
  dst_height = src_height + 2 * sampling_radius;

  texture = cogl_texture_2d_new_with_size (cogl_context, dst_width, dst_height);
  if (!texture)
    return NULL;

  offscreen = cogl_offscreen_new_with_texture (texture);
  fb = COGL_FRAMEBUFFER (offscreen);
  if (!cogl_framebuffer_allocate (fb, &error))
    {
      g_clear_object (&texture);
      return NULL;
    }

  cogl_framebuffer_clear4f (fb, COGL_BUFFER_BIT_COLOR, 0.f, 0.f, 0.f, 0.f);
  cogl_framebuffer_orthographic (fb, 0, 0, dst_width, dst_height, 0, 1.0);

  /* Blur */
  blur_node = clutter_blur_node_new (dst_width, dst_height, radius);
  clutter_paint_node_add_rectangle (blur_node,
                                    &(ClutterActorBox) {
                                      0.f, 0.f,
                                      dst_width, dst_height,
                                    });

  /* Texture */
  texture_pipeline = cogl_context_get_named_pipeline (cogl_context,
                                                      &texture_pipeline_key);

  if (G_UNLIKELY (texture_pipeline == NULL))
    {
      CoglSnippet *snippet;

      snippet = cogl_snippet_new (COGL_SNIPPET_HOOK_FRAGMENT,
                                  "",
                                  "if (cogl_color_out.a > 0.0)\n"
                                  "  cogl_color_out.a = 1.0;");

      texture_pipeline = cogl_pipeline_new (cogl_context);
      cogl_pipeline_add_snippet (texture_pipeline, snippet);
      g_object_unref (snippet);

      cogl_context_set_named_pipeline (cogl_context,
                                       &texture_pipeline_key,
                                       texture_pipeline);
    }

  /* No need to unref texture_pipeline since the named pipeline hash
   * doesn't change its ref count from 1. Also no need to copy texture_pipeline
   * since we'll be completely finished with it after clutter_paint_node_paint.
   */

  cogl_pipeline_set_layer_texture (texture_pipeline, 0, src_texture);
  texture_node = clutter_pipeline_node_new (texture_pipeline);
  clutter_paint_node_add_child (blur_node, texture_node);
  clutter_paint_node_add_rectangle (texture_node,
                                    &(ClutterActorBox) {
                                      .x1 = sampling_radius,
                                      .y1 = sampling_radius,
                                      .x2 = src_width + sampling_radius,
                                      .y2 = src_height + sampling_radius,
                                    });

  color_state = clutter_paint_context_get_color_state (paint_context);
  nested_paint_context =
    clutter_paint_context_new_for_framebuffer (fb, NULL, CLUTTER_PAINT_FLAG_NONE,
                                               color_state);
  clutter_paint_context_push_color_state (nested_paint_context, color_state);
  clutter_paint_node_paint (blur_node, nested_paint_context);
  clutter_paint_context_pop_color_state (nested_paint_context);
  clutter_paint_context_destroy (nested_paint_context);

  if (G_UNLIKELY (shadow_pipeline_template == NULL))
    {
      shadow_pipeline_template = cogl_pipeline_new (cogl_context);

      /* We set up the pipeline to blend the shadow texture with the combine
       * constant, but defer setting the latter until painting, so that we can
       * take the actor's overall opacity into account. */
      cogl_pipeline_set_layer_combine (shadow_pipeline_template, 0,
                                       "RGBA = MODULATE (CONSTANT, TEXTURE[A])",
                                       NULL);
    }

  pipeline = cogl_pipeline_copy (shadow_pipeline_template);
  cogl_pipeline_set_layer_texture (pipeline, 0, texture);

  g_clear_object (&texture);

  return pipeline;
}

CoglPipeline *
_st_create_shadow_pipeline_from_actor (StShadow            *shadow_spec,
                                        ClutterActor        *actor,
                                        ClutterPaintContext *paint_context)
{
  ClutterContent *image = NULL;
  CoglPipeline *shadow_pipeline = NULL;
  float resource_scale;
  float width, height;
  ClutterColorState *color_state;
  ClutterPaintContext *nested_paint_context;

  g_return_val_if_fail (clutter_actor_has_allocation (actor), NULL);

  clutter_actor_get_size (actor, &width, &height);

  if (width == 0 || height == 0)
    return NULL;

  resource_scale = clutter_actor_get_resource_scale (actor);

  width = ceilf (width * resource_scale);
  height = ceilf (height * resource_scale);

  image = clutter_actor_get_content (actor);
  if (image && ST_IS_IMAGE_CONTENT (image))
    {
      CoglTexture *texture;

      texture = st_image_content_get_texture (ST_IMAGE_CONTENT (image));
      if (texture &&
          cogl_texture_get_width (texture) == width &&
          cogl_texture_get_height (texture) == height)
        {
          shadow_pipeline = _st_create_shadow_pipeline (shadow_spec,
                                                        paint_context,
                                                        texture,
                                                        resource_scale);
        }
    }

  if (shadow_pipeline == NULL)
    {
      g_autoptr(CoglTexture) buffer = NULL;
      CoglOffscreen *offscreen;
      CoglFramebuffer *fb;
      ClutterBackend *backend;
      ClutterContext *context;
      CoglContext *cogl_context;
      CoglColor clear_color;
      GError *catch_error = NULL;
      float x, y;

      context = clutter_actor_get_context (actor);
      backend = clutter_context_get_backend (context);
      cogl_context = clutter_backend_get_cogl_context (backend);
      buffer = cogl_texture_2d_new_with_size (cogl_context, width, height);

      if (buffer == NULL)
        return NULL;

      offscreen = cogl_offscreen_new_with_texture (buffer);
      fb = COGL_FRAMEBUFFER (offscreen);

      if (!cogl_framebuffer_allocate (fb, &catch_error))
        {
          g_error_free (catch_error);
          g_object_unref (offscreen);
          return NULL;
        }

      cogl_color_init_from_4f (&clear_color, 0.0, 0.0, 0.0, 0.0);
      clutter_actor_get_position (actor, &x, &y);
      x *= resource_scale;
      y *= resource_scale;

      cogl_framebuffer_clear (fb, COGL_BUFFER_BIT_COLOR, &clear_color);
      cogl_framebuffer_translate (fb, -x, -y, 0);
      cogl_framebuffer_orthographic (fb, 0, 0, width, height, 0, 1.0);
      cogl_framebuffer_scale (fb, resource_scale, resource_scale, 1);

      clutter_actor_set_opacity_override (actor, 255);

      color_state = clutter_actor_get_color_state (actor);
      nested_paint_context =
        clutter_paint_context_new_for_framebuffer (fb, NULL,
                                                   CLUTTER_PAINT_FLAG_NONE,
                                                   color_state);
      clutter_paint_context_push_color_state (nested_paint_context, color_state);
      clutter_actor_paint (actor, nested_paint_context);
      clutter_paint_context_pop_color_state (nested_paint_context);
      clutter_paint_context_destroy (nested_paint_context);

      clutter_actor_set_opacity_override (actor, -1);

      g_object_unref (fb);

      shadow_pipeline = _st_create_shadow_pipeline (shadow_spec,
                                                    paint_context,
                                                    buffer,
                                                    resource_scale);
    }

  return shadow_pipeline;
}
