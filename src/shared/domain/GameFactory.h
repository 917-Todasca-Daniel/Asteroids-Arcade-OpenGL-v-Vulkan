#pragma once

#include <vector>

#define FACTORY aa::GameFactory::getFactory()


namespace aa
{

    class Object3d;
    class Mesh;

    class VKPipeline;
    class VKVertexShader;
    class VKShader;
    class VKTexture;

    class GLTexture;
    class GLShader;


    // interface for object factories
    class GameFactory
    {

    public:
        GameFactory();
        virtual ~GameFactory();

        virtual Object3d* buildSky(float height, float width, float *time) = 0;
        virtual Object3d* buildLargeAsteroid()                             = 0;

        static GameFactory* getFactory();

        //  delete all implicit constructors 
        GameFactory(const GameFactory&) = delete;
        GameFactory(GameFactory&&)      = delete;

        GameFactory& operator = (const GameFactory&) = delete;
        GameFactory& operator = (GameFactory&&)      = delete;


    protected:
        Object3d* buildLargeAsteroid(Mesh* mesh);


    private:
        static GameFactory* instance;


    };


    // concrete implementation for openGL Graphics
    // built object must call init
    class OpenGLGraphicsFactory final : public GameFactory {
    public:
        OpenGLGraphicsFactory();
        virtual ~OpenGLGraphicsFactory();

        Object3d* buildSky(float, float, float*);
        Object3d* buildLargeAsteroid();


    private:
        std::vector <GLShader*> shaders;
        GLTexture* asteroidTex;

    };


    // concrete implementation for Vulkan Graphics
    // built object must call init
    // vulkan-related resources are lazy loaded and freed on delete
    class VulkanGraphicsFactory final : public GameFactory {
    public:
        VulkanGraphicsFactory();
        virtual ~VulkanGraphicsFactory();

        Object3d* buildSky(float, float, float*);
        Object3d* buildLargeAsteroid();


    private:
        VKTexture*      tex;

        VKVertexShader* skyVertexShader;
        VKVertexShader* meshVertexShader;

        VKShader*       skyFragmentShader;
        VKShader*       meshFragmentShader;

        VKPipeline*     skyPipeline;
        VKPipeline*     meshPipeline;

        void buildSkyPrereq();
        void buildMeshPrereq();

    };

}
