
# Create the extension library
ADD_LIBRARY(Extensions_PostProcessing
  PostProcessing/PostProcessingException.cpp
  PostProcessing/OpenGL/PostProcessingEffect.cpp
  PostProcessing/OpenGL/PostProcessingPass.cpp
  Resources/PPEResourceException.cpp
  Resources/OpenGL/FragmentProgram.cpp
  Resources/OpenGL/FramebufferObject.cpp
  Resources/OpenGL/RenderBuffer.cpp
  Resources/OpenGL/Texture2D.cpp
  Resources/OpenGL/TextureCube.cpp
  Renderers/OpenGL/PostProcessingRenderingView.cpp
  Scene/BlendNode.cpp
  Scene/MergeNode.cpp
  Scene/MergeBlendNode.cpp
)

TARGET_LINK_LIBRARIES(Extensions_PostProcessing
  # depending libraries
  OpenEngine_Resources
  OpenEngine_Scene
)