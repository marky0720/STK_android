LOCAL_PATH := $(call my-dir)

# --------------------------------------------------------------------------
# irrlicht ¿â 
# 
 

include $(CLEAR_VARS) 
 
 
IRRMESHLOADER = CBSPMeshFileLoader.cpp CMD2MeshFileLoader.cpp CMD3MeshFileLoader.cpp CMS3DMeshFileLoader.cpp CB3DMeshFileLoader.cpp C3DSMeshFileLoader.cpp COgreMeshFileLoader.cpp COBJMeshFileLoader.cpp CColladaFileLoader.cpp CCSMLoader.cpp CDMFLoader.cpp CLMTSMeshFileLoader.cpp CMY3DMeshFileLoader.cpp COCTLoader.cpp CXMeshFileLoader.cpp CIrrMeshFileLoader.cpp CSTLMeshFileLoader.cpp CLWOMeshFileLoader.cpp CPLYMeshFileLoader.cpp
IRRMESHWRITER = CColladaMeshWriter.cpp CIrrMeshWriter.cpp CSTLMeshWriter.cpp COBJMeshWriter.cpp CPLYMeshWriter.cpp
IRRMESHOBJ = $(IRRMESHLOADER) $(IRRMESHWRITER) CSkinnedMesh.cpp CBoneSceneNode.cpp CMeshSceneNode.cpp CAnimatedMeshSceneNode.cpp CAnimatedMeshMD2.cpp CAnimatedMeshMD3.cpp CQ3LevelMesh.cpp CQuake3ShaderSceneNode.cpp
IRROBJ = CBillboardSceneNode.cpp CCameraSceneNode.cpp CDummyTransformationSceneNode.cpp CEmptySceneNode.cpp CGeometryCreator.cpp CLightSceneNode.cpp CMeshManipulator.cpp CMetaTriangleSelector.cpp COctreeSceneNode.cpp COctreeTriangleSelector.cpp CSceneCollisionManager.cpp CSceneManager.cpp CShadowVolumeSceneNode.cpp CSkyBoxSceneNode.cpp CSkyDomeSceneNode.cpp CTerrainSceneNode.cpp CTerrainTriangleSelector.cpp CVolumeLightSceneNode.cpp CCubeSceneNode.cpp CSphereSceneNode.cpp CTextSceneNode.cpp CTriangleBBSelector.cpp CTriangleSelector.cpp CWaterSurfaceSceneNode.cpp CMeshCache.cpp CDefaultSceneNodeAnimatorFactory.cpp CDefaultSceneNodeFactory.cpp
IRRPARTICLEOBJ = CParticleAnimatedMeshSceneNodeEmitter.cpp CParticleBoxEmitter.cpp CParticleCylinderEmitter.cpp CParticleMeshEmitter.cpp CParticlePointEmitter.cpp CParticleRingEmitter.cpp CParticleSphereEmitter.cpp CParticleAttractionAffector.cpp CParticleFadeOutAffector.cpp CParticleGravityAffector.cpp CParticleRotationAffector.cpp CParticleSystemSceneNode.cpp CParticleScaleAffector.cpp
IRRANIMOBJ = CSceneNodeAnimatorCameraFPS.cpp CSceneNodeAnimatorCameraMaya.cpp CSceneNodeAnimatorCollisionResponse.cpp CSceneNodeAnimatorDelete.cpp CSceneNodeAnimatorFlyCircle.cpp CSceneNodeAnimatorFlyStraight.cpp CSceneNodeAnimatorFollowSpline.cpp CSceneNodeAnimatorRotation.cpp CSceneNodeAnimatorTexture.cpp
IRRDRVROBJ = CNullDriver.cpp COGLESDriver.cpp COGLESTexture.cpp COGLESExtensionHandler.cpp
IRRDRVOBJ2 = COGLES2Driver.cpp COGLES2FixedPipelineShader.cpp COGLES2ParallaxMapRenderer.cpp COGLES2SLMaterialRenderer.cpp COGLES2ExtensionHandler.cpp COGLES2NormalMapRenderer.cpp COGLES2Renderer2D.cpp COGLES2Texture.cpp

IRRIMAGEOBJ = CColorConverter.cpp CImage.cpp CImageLoaderBMP.cpp CImageLoaderJPG.cpp CImageLoaderPCX.cpp CImageLoaderPNG.cpp CImageLoaderPSD.cpp CImageLoaderTGA.cpp CImageLoaderPPM.cpp CImageLoaderWAL.cpp CImageLoaderRGB.cpp CImageWriterBMP.cpp CImageWriterJPG.cpp CImageWriterPCX.cpp CImageWriterPNG.cpp CImageWriterPPM.cpp CImageWriterPSD.cpp CImageWriterTGA.cpp
IRRVIDEOOBJ = CVideoModeList.cpp CFPSCounter.cpp $(IRRDRVROBJ) $(IRRIMAGEOBJ)
IRRSWRENDEROBJ = CSoftwareDriver.cpp CSoftwareTexture.cpp CTRFlat.cpp CTRFlatWire.cpp CTRGouraud.cpp CTRGouraudWire.cpp CTRTextureFlat.cpp CTRTextureFlatWire.cpp CTRTextureGouraud.cpp CTRTextureGouraudAdd.cpp CTRTextureGouraudNoZ.cpp CTRTextureGouraudWire.cpp CZBuffer.cpp CTRTextureGouraudVertexAlpha2.cpp CTRTextureGouraudNoZ2.cpp CTRTextureLightMap2_M2.cpp CTRTextureLightMap2_M4.cpp CTRTextureLightMap2_M1.cpp CSoftwareDriver2.cpp CSoftwareTexture2.cpp CTRTextureGouraud2.cpp CTRGouraud2.cpp CTRGouraudAlpha2.cpp CTRGouraudAlphaNoZ2.cpp CTRTextureDetailMap2.cpp CTRTextureGouraudAdd2.cpp CTRTextureGouraudAddNoZ2.cpp CTRTextureWire2.cpp CTRTextureLightMap2_Add.cpp CTRTextureLightMapGouraud2_M4.cpp IBurningShader.cpp CTRTextureBlend.cpp CTRTextureGouraudAlpha.cpp CTRTextureGouraudAlphaNoZ.cpp CDepthBuffer.cpp CBurningShader_Raster_Reference.cpp
IRRIOOBJ = CFileList.cpp CFileSystem.cpp CLimitReadFile.cpp CMemoryFile.cpp CReadFile.cpp CWriteFile.cpp CXMLReader.cpp CXMLWriter.cpp CZipReader.cpp CPakReader.cpp CNPKReader.cpp CTarReader.cpp CMountPointReader.cpp irrXML.cpp CAttributes.cpp lzma/LzmaDec.c
IRROTHEROBJ = CIrrDeviceConsole.cpp CIrrDeviceStub.cpp CLogger.cpp Irrlicht.cpp os.cpp
IRRGUIOBJ = CGUIButton.cpp CGUICheckBox.cpp CGUIComboBox.cpp CGUIContextMenu.cpp CGUIEditBox.cpp CGUIEnvironment.cpp CGUIFileOpenDialog.cpp CGUIFont.cpp CGUIImage.cpp CGUIInOutFader.cpp CGUIListBox.cpp CGUIMenu.cpp CGUIMeshViewer.cpp CGUIMessageBox.cpp CGUIModalScreen.cpp CGUIScrollBar.cpp CGUISpinBox.cpp CGUISkin.cpp CGUIStaticText.cpp CGUITabControl.cpp CGUITable.cpp CGUIToolBar.cpp CGUIWindow.cpp CGUIColorSelectDialog.cpp CDefaultGUIElementFactory.cpp CGUISpriteBank.cpp CGUIImageList.cpp CGUITreeView.cpp

ZLIBOBJ = zlib/adler32.c zlib/compress.c zlib/crc32.c zlib/deflate.c zlib/inffast.c zlib/inflate.c zlib/inftrees.c zlib/trees.c zlib/uncompr.c zlib/zutil.c
JPEGLIBOBJ = jpeglib/jcapimin.c jpeglib/jcapistd.c jpeglib/jccoefct.c jpeglib/jccolor.c jpeglib/jcdctmgr.c jpeglib/jchuff.c jpeglib/jcinit.c jpeglib/jcmainct.c jpeglib/jcmarker.c jpeglib/jcmaster.c jpeglib/jcomapi.c jpeglib/jcparam.c jpeglib/jcprepct.c jpeglib/jcsample.c jpeglib/jctrans.c jpeglib/jdapimin.c jpeglib/jdapistd.c jpeglib/jdatadst.c jpeglib/jdatasrc.c jpeglib/jdcoefct.c jpeglib/jdcolor.c jpeglib/jddctmgr.c jpeglib/jdhuff.c jpeglib/jdinput.c jpeglib/jdmainct.c jpeglib/jdmarker.c jpeglib/jdmaster.c jpeglib/jdmerge.c jpeglib/jdpostct.c jpeglib/jdsample.c jpeglib/jdtrans.c jpeglib/jerror.c jpeglib/jfdctflt.c jpeglib/jfdctfst.c jpeglib/jfdctint.c jpeglib/jidctflt.c jpeglib/jidctfst.c jpeglib/jidctint.c jpeglib/jmemmgr.c jpeglib/jmemnobs.c jpeglib/jquant1.c jpeglib/jquant2.c jpeglib/jutils.c jpeglib/jcarith.c jpeglib/jdarith.c jpeglib/jaricom.c
LIBPNGOBJ = libpng/png.c libpng/pngerror.c libpng/pngget.c libpng/pngmem.c libpng/pngpread.c libpng/pngread.c libpng/pngrio.c libpng/pngrtran.c libpng/pngrutil.c libpng/pngset.c libpng/pngtrans.c libpng/pngwio.c libpng/pngwrite.c libpng/pngwtran.c libpng/pngwutil.c
LIBAESGM = aesGladman/aescrypt.cpp aesGladman/aeskey.cpp aesGladman/aestab.cpp aesGladman/fileenc.cpp aesGladman/hmac.cpp aesGladman/prng.cpp aesGladman/pwd2key.cpp aesGladman/sha1.cpp aesGladman/sha2.cpp
BZIP2OBJ = bzip2/blocksort.c bzip2/huffman.c bzip2/crctable.c bzip2/randtable.c bzip2/bzcompress.c bzip2/decompress.c bzip2/bzlib.c
 
  
 
ENET =enet/callbacks.cpp enet/host.cpp enet/list.cpp enet/packet.cpp enet/peer.cpp enet/protocol.cpp enet/unix.cpp  \

   
LOCAL_MODULE := irrlicht  
##add by marky
#LOCAL_MODULE_FILENAME := libirrlicht        
                    
#LOCAL_ARM_MODE   := arm 
LOCAL_CFLAGS := -O3 -DANDROID_NDK -DDISABLE_IMPORTGL -I./../include/ -I./include/ 

#LOCAL_CPPFLAGS += -fexceptions
				 
#LOCAL_C_INCLUDES = $(INCLUDES) ##modify by marky
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/../include \

LOCAL_SRC_FILES := CIrrDeviceAndroid.cpp \
	 $(ENET)  \
     $(IRRVIDEOOBJ) $(IRRDRVOBJ2) \
     $(IRRMESHOBJ) $(IRROBJ) $(IRRPARTICLEOBJ) $(IRRANIMOBJ) \
     $(IRRSWRENDEROBJ) $(IRRIOOBJ) $(IRROTHEROBJ) \
     $(IRRGUIOBJ) $(ZLIBOBJ) $(JPEGLIBOBJ) $(LIBPNGOBJ) $(LIBAESGM) \
     $(BZIP2OBJ)
 
LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog -lGLESv2
  
include $(BUILD_STATIC_LIBRARY) 
 

 

# --------------------------------------------------------------------------


# 
# COSOperator.cpp was removed from IRROTHEROBJ
  
include $(CLEAR_VARS)
 
IRR = importgl.cpp app-android.cpp android-activity.cpp android-receiver.cpp 
 
	
LOCAL_MODULE := irr
#LOCAL_MODULE_FILENAME := libirr    

#LOCAL_ARM_MODE   := arm 
LOCAL_CFLAGS := -O3 -DANDROID_NDK -DDISABLE_IMPORTGL -I./../include/ -I./include/
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/../include \
 
 
 
BULLET = LinearMath/btAlignedAllocator.cpp LinearMath/btConvexHull.cpp  \
	LinearMath/btGeometryUtil.cpp  LinearMath/btQuickprof.cpp \
 	BulletDynamics/ConstraintSolver/btConeTwistConstraint.cpp    \
	BulletDynamics/ConstraintSolver/btContactConstraint.cpp \
	BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.cpp \
	BulletDynamics/ConstraintSolver/btHingeConstraint.cpp \
	BulletDynamics/ConstraintSolver/btOdeContactJoint.cpp \
	BulletDynamics/ConstraintSolver/btOdeJoint.cpp  \
	BulletDynamics/ConstraintSolver/btOdeQuickstepConstraintSolver.cpp \
	BulletDynamics/ConstraintSolver/btOdeTypedJoint.cpp  \
	BulletDynamics/ConstraintSolver/btPoint2PointConstraint.cpp \
	BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.cpp \
	BulletDynamics/ConstraintSolver/btSliderConstraint.cpp \
	BulletDynamics/ConstraintSolver/btSolve2LinearConstraint.cpp \
	BulletDynamics/ConstraintSolver/btSorLcp.cpp \
	BulletDynamics/ConstraintSolver/btTypedConstraint.cpp \
	BulletDynamics/Dynamics/btContinuousDynamicsWorld.cpp \
	BulletDynamics/Dynamics/btDiscreteDynamicsWorld.cpp \
	BulletDynamics/Dynamics/btRigidBody.cpp \
	BulletDynamics/Dynamics/btSimpleDynamicsWorld.cpp \
	BulletDynamics/Dynamics/Bullet-C-API.cpp \
	BulletDynamics/Vehicle/btRaycastVehicle.cpp \
	BulletDynamics/Vehicle/btWheelInfo.cpp \
	BulletCollision/BroadphaseCollision/btAxisSweep3.cpp \
	BulletCollision/BroadphaseCollision/btBroadphaseProxy.cpp \
	BulletCollision/BroadphaseCollision/btCollisionAlgorithm.cpp \
	BulletCollision/BroadphaseCollision/btDispatcher.cpp \
	BulletCollision/BroadphaseCollision/btMultiSapBroadphase.cpp \
	BulletCollision/BroadphaseCollision/btOverlappingPairCache.cpp \
	BulletCollision/BroadphaseCollision/btSimpleBroadphase.cpp \
	BulletCollision/CollisionDispatch/btBoxBoxCollisionAlgorithm.cpp \
	BulletCollision/CollisionDispatch/btBoxBoxDetector.cpp \
	BulletCollision/CollisionDispatch/btCollisionDispatcher.cpp \
	BulletCollision/CollisionDispatch/btCollisionObject.cpp \
	BulletCollision/CollisionDispatch/btCollisionWorld.cpp \
	BulletCollision/CollisionDispatch/btCompoundCollisionAlgorithm.cpp \
	BulletCollision/CollisionDispatch/btConvexConcaveCollisionAlgorithm.cpp \
	BulletCollision/CollisionDispatch/btConvexConvexAlgorithm.cpp \
	BulletCollision/CollisionDispatch/btConvexPlaneCollisionAlgorithm.cpp \
	BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.cpp \
	BulletCollision/CollisionDispatch/btEmptyCollisionAlgorithm.cpp \
	BulletCollision/CollisionDispatch/btManifoldResult.cpp \
	BulletCollision/CollisionDispatch/btSimulationIslandManager.cpp \
	BulletCollision/CollisionDispatch/btSphereBoxCollisionAlgorithm.cpp \
	BulletCollision/CollisionDispatch/btSphereSphereCollisionAlgorithm.cpp \
	BulletCollision/CollisionDispatch/btSphereTriangleCollisionAlgorithm.cpp \
	BulletCollision/CollisionDispatch/btUnionFind.cpp \
	BulletCollision/CollisionDispatch/SphereTriangleDetector.cpp \
	BulletCollision/CollisionShapes/btBoxShape.cpp \
	BulletCollision/CollisionShapes/btBvhTriangleMeshShape.cpp \
	BulletCollision/CollisionShapes/btCapsuleShape.cpp \
	BulletCollision/CollisionShapes/btCollisionShape.cpp \
	BulletCollision/CollisionShapes/btCompoundShape.cpp \
	BulletCollision/CollisionShapes/btConcaveShape.cpp \
	BulletCollision/CollisionShapes/btConeShape.cpp \
	BulletCollision/CollisionShapes/btConvexHullShape.cpp \
	BulletCollision/CollisionShapes/btConvexInternalShape.cpp \
	BulletCollision/CollisionShapes/btConvexShape.cpp \
	BulletCollision/CollisionShapes/btConvexTriangleMeshShape.cpp \
	BulletCollision/CollisionShapes/btCylinderShape.cpp \
	BulletCollision/CollisionShapes/btEmptyShape.cpp \
	BulletCollision/CollisionShapes/btHeightfieldTerrainShape.cpp \
	BulletCollision/CollisionShapes/btMinkowskiSumShape.cpp \
	BulletCollision/CollisionShapes/btMultiSphereShape.cpp \
	BulletCollision/CollisionShapes/btOptimizedBvh.cpp \
	BulletCollision/CollisionShapes/btPolyhedralConvexShape.cpp \
	BulletCollision/CollisionShapes/btShapeHull.cpp \
	BulletCollision/CollisionShapes/btSphereShape.cpp \
	BulletCollision/CollisionShapes/btStaticPlaneShape.cpp \
	BulletCollision/CollisionShapes/btStridingMeshInterface.cpp \
	BulletCollision/CollisionShapes/btTetrahedronShape.cpp \
	BulletCollision/CollisionShapes/btTriangleBuffer.cpp \
	BulletCollision/CollisionShapes/btTriangleCallback.cpp \
	BulletCollision/CollisionShapes/btTriangleIndexVertexArray.cpp \
	BulletCollision/CollisionShapes/btTriangleMesh.cpp \
	BulletCollision/CollisionShapes/btTriangleMeshShape.cpp \
	BulletCollision/CollisionShapes/btUniformScalingShape.cpp \
	BulletCollision/NarrowPhaseCollision/btContinuousConvexCollision.cpp \
	BulletCollision/NarrowPhaseCollision/btConvexCast.cpp \
	BulletCollision/NarrowPhaseCollision/btGjkConvexCast.cpp \
	BulletCollision/NarrowPhaseCollision/btGjkEpa.cpp \
	BulletCollision/NarrowPhaseCollision/btGjkEpa2.cpp \
	BulletCollision/NarrowPhaseCollision/btGjkEpaPenetrationDepthSolver.cpp \
	BulletCollision/NarrowPhaseCollision/btGjkPairDetector.cpp \
	BulletCollision/NarrowPhaseCollision/btMinkowskiPenetrationDepthSolver.cpp \
	BulletCollision/NarrowPhaseCollision/btPersistentManifold.cpp \
	BulletCollision/NarrowPhaseCollision/btRaycastCallback.cpp \
	BulletCollision/NarrowPhaseCollision/btSubSimplexConvexCast.cpp \
	BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.cpp \
 
STK =main.cpp main_loop.cpp \
addons/addons.cpp  \
addons/network.cpp  \
addons/zip.cpp  \
animations/animation_base.cpp  \
animations/animation_manager.cpp  \
animations/billboard_animation.cpp  \
animations/ipo.cpp  \
animations/three_d_animation.cpp  \
audio/music_information.cpp  \
audio/music_manager.cpp  \
audio/music_ogg.cpp  \
audio/sfx_buffer.cpp  \
audio/sfx_manager.cpp  \
audio/sfx_openal.cpp  \
challenges/challenge.cpp  \
challenges/challenge_data.cpp  \
challenges/unlock_manager.cpp  \
config/device_config.cpp  \
config/stk_config.cpp  \
config/user_config.cpp  \
graphics/camera.cpp  \
graphics/CBatchingMesh.cpp  \
graphics/explosion.cpp  \
graphics/irr_driver.cpp  \
graphics/material.cpp  \
graphics/material_manager.cpp  \
graphics/mesh_tools.cpp  \
graphics/moving_texture.cpp  \
graphics/nitro.cpp  \
graphics/shadow.cpp  \
graphics/skid_marks.cpp  \
graphics/slip_stream.cpp  \
graphics/smoke.cpp  \
graphics/stars.cpp  \
graphics/water_splash.cpp  \
guiengine/abstract_state_manager.cpp  \
guiengine/abstract_top_level_container.cpp  \
guiengine/CGUISpriteBank.cpp  \
guiengine/engine.cpp  \
guiengine/event_handler.cpp  \
guiengine/layout_manager.cpp  \
guiengine/modaldialog.cpp  \
guiengine/scalable_font.cpp  \
guiengine/screen.cpp  \
guiengine/screen_loader.cpp  \
guiengine/skin.cpp  \
guiengine/widget.cpp  \
guiengine/widgets/bubble_widget.cpp  \
guiengine/widgets/button_widget.cpp  \
guiengine/widgets/check_box_widget.cpp  \
guiengine/widgets/dynamic_ribbon_widget.cpp  \
guiengine/widgets/icon_button_widget.cpp  \
guiengine/widgets/label_widget.cpp  \
guiengine/widgets/list_widget.cpp  \
guiengine/widgets/model_view_widget.cpp  \
guiengine/widgets/progress_bar_widget.cpp  \
guiengine/widgets/ribbon_widget.cpp  \
guiengine/widgets/spinner_widget.cpp  \
guiengine/widgets/text_box_widget.cpp  \
input/device_manager.cpp  \
input/input.cpp  \
input/input_device.cpp  \
input/input_manager.cpp  \
io/file_manager.cpp  \
io/xml_node.cpp  \
items/attachment.cpp  \
items/attachment_manager.cpp  \
items/bowling.cpp  \
items/cake.cpp  \
items/flyable.cpp  \
items/item.cpp  \
items/item_manager.cpp  \
items/plunger.cpp  \
items/powerup.cpp  \
items/powerup_manager.cpp  \
items/projectile_manager.cpp  \
items/rubber_band.cpp  \
karts/emergency_animation.cpp  \
karts/kart.cpp  \
karts/kart_model.cpp  \
karts/kart_properties.cpp  \
karts/kart_properties_manager.cpp  \
karts/max_speed.cpp  \
karts/moveable.cpp  \
karts/controller/ai_base_controller.cpp  \
karts/controller/controller.cpp  \
karts/controller/default_ai_controller.cpp  \
karts/controller/end_controller.cpp  \
karts/controller/new_ai_controller.cpp  \
karts/controller/player_controller.cpp  \
modes/follow_the_leader.cpp  \
modes/linear_world.cpp  \
modes/profile_world.cpp  \
modes/standard_race.cpp  \
modes/three_strikes_battle.cpp  \
modes/world.cpp  \
modes/world_status.cpp  \
modes/world_with_rank.cpp  \
network/connect_message.cpp  \
network/kart_control_message.cpp  \
network/kart_update_message.cpp  \
network/message.cpp  \
network/network_kart.cpp  \
network/network_manager.cpp  \
network/race_info_message.cpp  \
network/race_result_message.cpp  \
network/race_state.cpp  \
physics/btKart.cpp  \
physics/btUprightConstraint.cpp  \
physics/irr_debug_drawer.cpp  \
physics/physical_object.cpp  \
physics/physics.cpp  \
physics/triangle_mesh.cpp  \
race/grand_prix_data.cpp  \
race/grand_prix_manager.cpp  \
race/highscores.cpp  \
race/highscore_manager.cpp  \
race/history.cpp  \
race/race_manager.cpp  \
replay/replay_base.cpp  \
replay/replay_buffers.cpp  \
replay/replay_player.cpp  \
replay/replay_recorder.cpp  \
states_screens/addons_screen.cpp  \
states_screens/addons_update_screen.cpp  \
states_screens/arenas_screen.cpp  \
states_screens/challenges.cpp  \
states_screens/credits.cpp  \
states_screens/feature_unlocked.cpp  \
states_screens/grand_prix_lose.cpp  \
states_screens/grand_prix_win.cpp  \
states_screens/help_screen_1.cpp  \
states_screens/help_screen_2.cpp  \
states_screens/help_screen_3.cpp  \
states_screens/help_screen_4.cpp  \
states_screens/kart_selection.cpp  \
states_screens/main_menu_screen.cpp  \
states_screens/options_screen_audio.cpp  \
states_screens/options_screen_input.cpp  \
states_screens/options_screen_input2.cpp  \
states_screens/options_screen_players.cpp  \
states_screens/options_screen_video.cpp  \
states_screens/race_gui.cpp  \
states_screens/race_result_gui.cpp  \
states_screens/race_setup_screen.cpp  \
states_screens/state_manager.cpp  \
states_screens/tracks_screen.cpp  \
states_screens/dialogs/addons_loading.cpp  \
states_screens/dialogs/add_device_dialog.cpp  \
states_screens/dialogs/confirm_dialog.cpp  \
states_screens/dialogs/confirm_resolution_dialog.cpp  \
states_screens/dialogs/enter_player_name_dialog.cpp  \
states_screens/dialogs/gp_info_dialog.cpp  \
states_screens/dialogs/player_info_dialog.cpp  \
states_screens/dialogs/press_a_key_dialog.cpp  \
states_screens/dialogs/race_over_dialog.cpp  \
states_screens/dialogs/race_paused_dialog.cpp  \
states_screens/dialogs/track_info_dialog.cpp  \
tracks/ambient_light_sphere.cpp  \
tracks/bezier_curve.cpp  \
tracks/check_lap.cpp  \
tracks/check_line.cpp  \
tracks/check_manager.cpp  \
tracks/check_sphere.cpp  \
tracks/check_structure.cpp  \
tracks/graph_node.cpp  \
tracks/quad.cpp  \
tracks/quad_graph.cpp  \
tracks/quad_set.cpp  \
tracks/terrain_info.cpp  \
tracks/track.cpp  \
tracks/track_manager.cpp  \
tracks/track_object.cpp  \
tracks/track_object_manager.cpp  \
utils/constants.cpp  \
utils/random_generator.cpp  \
utils/string_utils.cpp  \
utils/translation.cpp  \
utils/vec3.cpp  \
 
LOCAL_SRC_FILES := 	 $(IRR) \
	 $(BULLET) \
     $(STK) \
     
LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog -lGLESv2 
 
LOCAL_STATIC_LIBRARIES := irrlicht
#LOCAL_STATIC_LIBRARIES += irr   

include $(BUILD_SHARED_LIBRARY)
 
