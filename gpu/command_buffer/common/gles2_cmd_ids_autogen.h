// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is auto-generated from
// gpu/command_buffer/build_gles2_cmd_buffer.py
// It's formatted by clang-format using chromium coding style:
//    clang-format -i -style=chromium filename
// DO NOT EDIT!

#ifndef GPU_COMMAND_BUFFER_COMMON_GLES2_CMD_IDS_AUTOGEN_H_
#define GPU_COMMAND_BUFFER_COMMON_GLES2_CMD_IDS_AUTOGEN_H_

#define GLES2_COMMAND_LIST(OP)                           \
  OP(ActiveTexture)                            /* 256 */ \
  OP(AttachShader)                             /* 257 */ \
  OP(BindAttribLocationBucket)                 /* 258 */ \
  OP(BindBuffer)                               /* 259 */ \
  OP(BindBufferBase)                           /* 260 */ \
  OP(BindBufferRange)                          /* 261 */ \
  OP(BindFramebuffer)                          /* 262 */ \
  OP(BindRenderbuffer)                         /* 263 */ \
  OP(BindSampler)                              /* 264 */ \
  OP(BindTexture)                              /* 265 */ \
  OP(BindTransformFeedback)                    /* 266 */ \
  OP(BlendColor)                               /* 267 */ \
  OP(BlendEquation)                            /* 268 */ \
  OP(BlendEquationSeparate)                    /* 269 */ \
  OP(BlendFunc)                                /* 270 */ \
  OP(BlendFuncSeparate)                        /* 271 */ \
  OP(BufferData)                               /* 272 */ \
  OP(BufferSubData)                            /* 273 */ \
  OP(CheckFramebufferStatus)                   /* 274 */ \
  OP(Clear)                                    /* 275 */ \
  OP(ClearBufferfi)                            /* 276 */ \
  OP(ClearBufferfvImmediate)                   /* 277 */ \
  OP(ClearBufferivImmediate)                   /* 278 */ \
  OP(ClearBufferuivImmediate)                  /* 279 */ \
  OP(ClearColor)                               /* 280 */ \
  OP(ClearDepthf)                              /* 281 */ \
  OP(ClearStencil)                             /* 282 */ \
  OP(ClientWaitSync)                           /* 283 */ \
  OP(ColorMask)                                /* 284 */ \
  OP(CompileShader)                            /* 285 */ \
  OP(CompressedTexImage2DBucket)               /* 286 */ \
  OP(CompressedTexImage2D)                     /* 287 */ \
  OP(CompressedTexSubImage2DBucket)            /* 288 */ \
  OP(CompressedTexSubImage2D)                  /* 289 */ \
  OP(CompressedTexImage3DBucket)               /* 290 */ \
  OP(CompressedTexImage3D)                     /* 291 */ \
  OP(CompressedTexSubImage3DBucket)            /* 292 */ \
  OP(CompressedTexSubImage3D)                  /* 293 */ \
  OP(CopyBufferSubData)                        /* 294 */ \
  OP(CopyTexImage2D)                           /* 295 */ \
  OP(CopyTexSubImage2D)                        /* 296 */ \
  OP(CopyTexSubImage3D)                        /* 297 */ \
  OP(CreateProgram)                            /* 298 */ \
  OP(CreateShader)                             /* 299 */ \
  OP(CullFace)                                 /* 300 */ \
  OP(DeleteBuffersImmediate)                   /* 301 */ \
  OP(DeleteFramebuffersImmediate)              /* 302 */ \
  OP(DeleteProgram)                            /* 303 */ \
  OP(DeleteRenderbuffersImmediate)             /* 304 */ \
  OP(DeleteSamplersImmediate)                  /* 305 */ \
  OP(DeleteSync)                               /* 306 */ \
  OP(DeleteShader)                             /* 307 */ \
  OP(DeleteTexturesImmediate)                  /* 308 */ \
  OP(DeleteTransformFeedbacksImmediate)        /* 309 */ \
  OP(DepthFunc)                                /* 310 */ \
  OP(DepthMask)                                /* 311 */ \
  OP(DepthRangef)                              /* 312 */ \
  OP(DetachShader)                             /* 313 */ \
  OP(Disable)                                  /* 314 */ \
  OP(DisableVertexAttribArray)                 /* 315 */ \
  OP(DrawArrays)                               /* 316 */ \
  OP(DrawElements)                             /* 317 */ \
  OP(Enable)                                   /* 318 */ \
  OP(EnableVertexAttribArray)                  /* 319 */ \
  OP(FenceSync)                                /* 320 */ \
  OP(Finish)                                   /* 321 */ \
  OP(Flush)                                    /* 322 */ \
  OP(FramebufferRenderbuffer)                  /* 323 */ \
  OP(FramebufferTexture2D)                     /* 324 */ \
  OP(FramebufferTextureLayer)                  /* 325 */ \
  OP(FrontFace)                                /* 326 */ \
  OP(GenBuffersImmediate)                      /* 327 */ \
  OP(GenerateMipmap)                           /* 328 */ \
  OP(GenFramebuffersImmediate)                 /* 329 */ \
  OP(GenRenderbuffersImmediate)                /* 330 */ \
  OP(GenSamplersImmediate)                     /* 331 */ \
  OP(GenTexturesImmediate)                     /* 332 */ \
  OP(GenTransformFeedbacksImmediate)           /* 333 */ \
  OP(GetActiveAttrib)                          /* 334 */ \
  OP(GetActiveUniform)                         /* 335 */ \
  OP(GetActiveUniformBlockiv)                  /* 336 */ \
  OP(GetActiveUniformBlockName)                /* 337 */ \
  OP(GetActiveUniformsiv)                      /* 338 */ \
  OP(GetAttachedShaders)                       /* 339 */ \
  OP(GetAttribLocation)                        /* 340 */ \
  OP(GetBooleanv)                              /* 341 */ \
  OP(GetBufferParameteriv)                     /* 342 */ \
  OP(GetError)                                 /* 343 */ \
  OP(GetFloatv)                                /* 344 */ \
  OP(GetFragDataLocation)                      /* 345 */ \
  OP(GetFramebufferAttachmentParameteriv)      /* 346 */ \
  OP(GetInteger64v)                            /* 347 */ \
  OP(GetIntegeri_v)                            /* 348 */ \
  OP(GetInteger64i_v)                          /* 349 */ \
  OP(GetIntegerv)                              /* 350 */ \
  OP(GetInternalformativ)                      /* 351 */ \
  OP(GetProgramiv)                             /* 352 */ \
  OP(GetProgramInfoLog)                        /* 353 */ \
  OP(GetRenderbufferParameteriv)               /* 354 */ \
  OP(GetSamplerParameterfv)                    /* 355 */ \
  OP(GetSamplerParameteriv)                    /* 356 */ \
  OP(GetShaderiv)                              /* 357 */ \
  OP(GetShaderInfoLog)                         /* 358 */ \
  OP(GetShaderPrecisionFormat)                 /* 359 */ \
  OP(GetShaderSource)                          /* 360 */ \
  OP(GetString)                                /* 361 */ \
  OP(GetSynciv)                                /* 362 */ \
  OP(GetTexParameterfv)                        /* 363 */ \
  OP(GetTexParameteriv)                        /* 364 */ \
  OP(GetTransformFeedbackVarying)              /* 365 */ \
  OP(GetUniformBlockIndex)                     /* 366 */ \
  OP(GetUniformfv)                             /* 367 */ \
  OP(GetUniformiv)                             /* 368 */ \
  OP(GetUniformIndices)                        /* 369 */ \
  OP(GetUniformLocation)                       /* 370 */ \
  OP(GetVertexAttribfv)                        /* 371 */ \
  OP(GetVertexAttribiv)                        /* 372 */ \
  OP(GetVertexAttribPointerv)                  /* 373 */ \
  OP(Hint)                                     /* 374 */ \
  OP(InvalidateFramebufferImmediate)           /* 375 */ \
  OP(InvalidateSubFramebufferImmediate)        /* 376 */ \
  OP(IsBuffer)                                 /* 377 */ \
  OP(IsEnabled)                                /* 378 */ \
  OP(IsFramebuffer)                            /* 379 */ \
  OP(IsProgram)                                /* 380 */ \
  OP(IsRenderbuffer)                           /* 381 */ \
  OP(IsSampler)                                /* 382 */ \
  OP(IsShader)                                 /* 383 */ \
  OP(IsSync)                                   /* 384 */ \
  OP(IsTexture)                                /* 385 */ \
  OP(IsTransformFeedback)                      /* 386 */ \
  OP(LineWidth)                                /* 387 */ \
  OP(LinkProgram)                              /* 388 */ \
  OP(PauseTransformFeedback)                   /* 389 */ \
  OP(PixelStorei)                              /* 390 */ \
  OP(PolygonOffset)                            /* 391 */ \
  OP(ReadBuffer)                               /* 392 */ \
  OP(ReadPixels)                               /* 393 */ \
  OP(ReleaseShaderCompiler)                    /* 394 */ \
  OP(RenderbufferStorage)                      /* 395 */ \
  OP(ResumeTransformFeedback)                  /* 396 */ \
  OP(SampleCoverage)                           /* 397 */ \
  OP(SamplerParameterf)                        /* 398 */ \
  OP(SamplerParameterfvImmediate)              /* 399 */ \
  OP(SamplerParameteri)                        /* 400 */ \
  OP(SamplerParameterivImmediate)              /* 401 */ \
  OP(Scissor)                                  /* 402 */ \
  OP(ShaderBinary)                             /* 403 */ \
  OP(ShaderSourceBucket)                       /* 404 */ \
  OP(StencilFunc)                              /* 405 */ \
  OP(StencilFuncSeparate)                      /* 406 */ \
  OP(StencilMask)                              /* 407 */ \
  OP(StencilMaskSeparate)                      /* 408 */ \
  OP(StencilOp)                                /* 409 */ \
  OP(StencilOpSeparate)                        /* 410 */ \
  OP(TexImage2D)                               /* 411 */ \
  OP(TexImage3D)                               /* 412 */ \
  OP(TexParameterf)                            /* 413 */ \
  OP(TexParameterfvImmediate)                  /* 414 */ \
  OP(TexParameteri)                            /* 415 */ \
  OP(TexParameterivImmediate)                  /* 416 */ \
  OP(TexStorage3D)                             /* 417 */ \
  OP(TexSubImage2D)                            /* 418 */ \
  OP(TexSubImage3D)                            /* 419 */ \
  OP(TransformFeedbackVaryingsBucket)          /* 420 */ \
  OP(Uniform1f)                                /* 421 */ \
  OP(Uniform1fvImmediate)                      /* 422 */ \
  OP(Uniform1i)                                /* 423 */ \
  OP(Uniform1ivImmediate)                      /* 424 */ \
  OP(Uniform1ui)                               /* 425 */ \
  OP(Uniform1uivImmediate)                     /* 426 */ \
  OP(Uniform2f)                                /* 427 */ \
  OP(Uniform2fvImmediate)                      /* 428 */ \
  OP(Uniform2i)                                /* 429 */ \
  OP(Uniform2ivImmediate)                      /* 430 */ \
  OP(Uniform2ui)                               /* 431 */ \
  OP(Uniform2uivImmediate)                     /* 432 */ \
  OP(Uniform3f)                                /* 433 */ \
  OP(Uniform3fvImmediate)                      /* 434 */ \
  OP(Uniform3i)                                /* 435 */ \
  OP(Uniform3ivImmediate)                      /* 436 */ \
  OP(Uniform3ui)                               /* 437 */ \
  OP(Uniform3uivImmediate)                     /* 438 */ \
  OP(Uniform4f)                                /* 439 */ \
  OP(Uniform4fvImmediate)                      /* 440 */ \
  OP(Uniform4i)                                /* 441 */ \
  OP(Uniform4ivImmediate)                      /* 442 */ \
  OP(Uniform4ui)                               /* 443 */ \
  OP(Uniform4uivImmediate)                     /* 444 */ \
  OP(UniformBlockBinding)                      /* 445 */ \
  OP(UniformMatrix2fvImmediate)                /* 446 */ \
  OP(UniformMatrix2x3fvImmediate)              /* 447 */ \
  OP(UniformMatrix2x4fvImmediate)              /* 448 */ \
  OP(UniformMatrix3fvImmediate)                /* 449 */ \
  OP(UniformMatrix3x2fvImmediate)              /* 450 */ \
  OP(UniformMatrix3x4fvImmediate)              /* 451 */ \
  OP(UniformMatrix4fvImmediate)                /* 452 */ \
  OP(UniformMatrix4x2fvImmediate)              /* 453 */ \
  OP(UniformMatrix4x3fvImmediate)              /* 454 */ \
  OP(UseProgram)                               /* 455 */ \
  OP(ValidateProgram)                          /* 456 */ \
  OP(VertexAttrib1f)                           /* 457 */ \
  OP(VertexAttrib1fvImmediate)                 /* 458 */ \
  OP(VertexAttrib2f)                           /* 459 */ \
  OP(VertexAttrib2fvImmediate)                 /* 460 */ \
  OP(VertexAttrib3f)                           /* 461 */ \
  OP(VertexAttrib3fvImmediate)                 /* 462 */ \
  OP(VertexAttrib4f)                           /* 463 */ \
  OP(VertexAttrib4fvImmediate)                 /* 464 */ \
  OP(VertexAttribI4i)                          /* 465 */ \
  OP(VertexAttribI4ivImmediate)                /* 466 */ \
  OP(VertexAttribI4ui)                         /* 467 */ \
  OP(VertexAttribI4uivImmediate)               /* 468 */ \
  OP(VertexAttribIPointer)                     /* 469 */ \
  OP(VertexAttribPointer)                      /* 470 */ \
  OP(Viewport)                                 /* 471 */ \
  OP(WaitSync)                                 /* 472 */ \
  OP(BlitFramebufferCHROMIUM)                  /* 473 */ \
  OP(RenderbufferStorageMultisampleCHROMIUM)   /* 474 */ \
  OP(RenderbufferStorageMultisampleEXT)        /* 475 */ \
  OP(FramebufferTexture2DMultisampleEXT)       /* 476 */ \
  OP(TexStorage2DEXT)                          /* 477 */ \
  OP(GenQueriesEXTImmediate)                   /* 478 */ \
  OP(DeleteQueriesEXTImmediate)                /* 479 */ \
  OP(BeginQueryEXT)                            /* 480 */ \
  OP(BeginTransformFeedback)                   /* 481 */ \
  OP(EndQueryEXT)                              /* 482 */ \
  OP(EndTransformFeedback)                     /* 483 */ \
  OP(InsertEventMarkerEXT)                     /* 484 */ \
  OP(PushGroupMarkerEXT)                       /* 485 */ \
  OP(PopGroupMarkerEXT)                        /* 486 */ \
  OP(GenVertexArraysOESImmediate)              /* 487 */ \
  OP(DeleteVertexArraysOESImmediate)           /* 488 */ \
  OP(IsVertexArrayOES)                         /* 489 */ \
  OP(BindVertexArrayOES)                       /* 490 */ \
  OP(SwapBuffers)                              /* 491 */ \
  OP(GetMaxValueInBufferCHROMIUM)              /* 492 */ \
  OP(EnableFeatureCHROMIUM)                    /* 493 */ \
  OP(MapBufferRange)                           /* 494 */ \
  OP(UnmapBuffer)                              /* 495 */ \
  OP(ResizeCHROMIUM)                           /* 496 */ \
  OP(GetRequestableExtensionsCHROMIUM)         /* 497 */ \
  OP(RequestExtensionCHROMIUM)                 /* 498 */ \
  OP(GetProgramInfoCHROMIUM)                   /* 499 */ \
  OP(GetUniformBlocksCHROMIUM)                 /* 500 */ \
  OP(GetTransformFeedbackVaryingsCHROMIUM)     /* 501 */ \
  OP(GetUniformsES3CHROMIUM)                   /* 502 */ \
  OP(GetTranslatedShaderSourceANGLE)           /* 503 */ \
  OP(PostSubBufferCHROMIUM)                    /* 504 */ \
  OP(TexImageIOSurface2DCHROMIUM)              /* 505 */ \
  OP(CopyTextureCHROMIUM)                      /* 506 */ \
  OP(CopySubTextureCHROMIUM)                   /* 507 */ \
  OP(DrawArraysInstancedANGLE)                 /* 508 */ \
  OP(DrawElementsInstancedANGLE)               /* 509 */ \
  OP(VertexAttribDivisorANGLE)                 /* 510 */ \
  OP(GenMailboxCHROMIUM)                       /* 511 */ \
  OP(ProduceTextureCHROMIUMImmediate)          /* 512 */ \
  OP(ProduceTextureDirectCHROMIUMImmediate)    /* 513 */ \
  OP(ConsumeTextureCHROMIUMImmediate)          /* 514 */ \
  OP(CreateAndConsumeTextureCHROMIUMImmediate) /* 515 */ \
  OP(BindUniformLocationCHROMIUMBucket)        /* 516 */ \
  OP(GenValuebuffersCHROMIUMImmediate)         /* 517 */ \
  OP(DeleteValuebuffersCHROMIUMImmediate)      /* 518 */ \
  OP(IsValuebufferCHROMIUM)                    /* 519 */ \
  OP(BindValuebufferCHROMIUM)                  /* 520 */ \
  OP(SubscribeValueCHROMIUM)                   /* 521 */ \
  OP(PopulateSubscribedValuesCHROMIUM)         /* 522 */ \
  OP(UniformValuebufferCHROMIUM)               /* 523 */ \
  OP(BindTexImage2DCHROMIUM)                   /* 524 */ \
  OP(ReleaseTexImage2DCHROMIUM)                /* 525 */ \
  OP(TraceBeginCHROMIUM)                       /* 526 */ \
  OP(TraceEndCHROMIUM)                         /* 527 */ \
  OP(AsyncTexSubImage2DCHROMIUM)               /* 528 */ \
  OP(AsyncTexImage2DCHROMIUM)                  /* 529 */ \
  OP(WaitAsyncTexImage2DCHROMIUM)              /* 530 */ \
  OP(WaitAllAsyncTexImage2DCHROMIUM)           /* 531 */ \
  OP(DiscardFramebufferEXTImmediate)           /* 532 */ \
  OP(LoseContextCHROMIUM)                      /* 533 */ \
  OP(InsertSyncPointCHROMIUM)                  /* 534 */ \
  OP(WaitSyncPointCHROMIUM)                    /* 535 */ \
  OP(DrawBuffersEXTImmediate)                  /* 536 */ \
  OP(DiscardBackbufferCHROMIUM)                /* 537 */ \
  OP(ScheduleOverlayPlaneCHROMIUM)             /* 538 */ \
  OP(SwapInterval)                             /* 539 */ \
  OP(MatrixLoadfCHROMIUMImmediate)             /* 540 */ \
  OP(MatrixLoadIdentityCHROMIUM)               /* 541 */ \
  OP(BlendBarrierKHR)                          /* 542 */

enum CommandId {
  kStartPoint = cmd::kLastCommonId,  // All GLES2 commands start after this.
#define GLES2_CMD_OP(name) k##name,
  GLES2_COMMAND_LIST(GLES2_CMD_OP)
#undef GLES2_CMD_OP
      kNumCommands
};

#endif  // GPU_COMMAND_BUFFER_COMMON_GLES2_CMD_IDS_AUTOGEN_H_
