#pragma once

#include <vector>

#define FACTORY         aa::GameFactory     ::getFactory()
#define SHIP_FACTORY    aa::SpaceshipFactory::getFactory()
#define LASER_FACTORY   aa::LaserFactory    ::getInstance()


namespace aa
{

    class Laser;

    class Object3d;
    class Mesh;

    class VKPipeline;
    class VKVertexShader;
    class VKShader;
    class VKTexture;
    class VKInstancedMesh;

    class GLTexture;
    class GLShader;
    class GLInstancedMesh;

    class CollisionShape;


    // interface for ship factories
    class SpaceshipFactory
    {

    public:
        SpaceshipFactory();
        virtual ~SpaceshipFactory();

        virtual Object3d* buildSpaceship() = 0;

        static SpaceshipFactory* getFactory();

        //  delete all implicit constructors 
        SpaceshipFactory(const SpaceshipFactory&) = delete;
        SpaceshipFactory(SpaceshipFactory&&)      = delete;

        SpaceshipFactory& operator = (const SpaceshipFactory&) = delete;
        SpaceshipFactory& operator = (SpaceshipFactory&&)      = delete;


    protected:
        Object3d* buildSpaceship(Mesh* mesh);


    private:
        Object3d*       spaceship;
        CollisionShape* shipCollision;

        static SpaceshipFactory* instance;

        friend class GameFactory;

    };

    // concrete OpenGL factory implementation
    class OpenGLSpaceshipFactory final : public SpaceshipFactory
    {

    public:
        OpenGLSpaceshipFactory();
        virtual ~OpenGLSpaceshipFactory();

        virtual Object3d* buildSpaceship();


    private:
        std::vector <GLShader*> shaders;
        GLTexture* shipTex;

    };

    // concrete Vulkan factory implementation
    class VulkanSpaceshipFactory final : public SpaceshipFactory
    {

    public:
        VulkanSpaceshipFactory();
        virtual ~VulkanSpaceshipFactory();

        virtual Object3d* buildSpaceship();


    private:
        VKTexture* shipTex;
        std::vector <VKShader*>   shaders;
        std::vector <VKPipeline*> pipelines;

    };


    // interface for laser factories
    class LaserFactory
    {

    public:
        LaserFactory();
        ~LaserFactory();

        std::vector<Laser*> buildLasers();

        static LaserFactory* getInstance();

        void draw();

        //  delete all implicit constructors 
        LaserFactory(const LaserFactory&) = delete;
        LaserFactory(LaserFactory&&) = delete;

        LaserFactory& operator = (const LaserFactory&) = delete;
        LaserFactory& operator = (LaserFactory&&) = delete;


    protected:
        std::vector <Laser*> lasers;

        static LaserFactory* instance;

        virtual Mesh* createMesh() = 0;

        friend class GameFactory;

    };

    class OpenGLLaserFactory : LaserFactory
    {

    public:
        OpenGLLaserFactory();
        ~OpenGLLaserFactory();


    protected:
        virtual Mesh* createMesh();

        std::vector <GLShader*> shaders;

    };

    class VulkanLaserFactory : LaserFactory
    {

    public:
        VulkanLaserFactory();
        ~VulkanLaserFactory();


    protected:
        virtual Mesh* createMesh();

        std::vector <VKShader*>     shaders;
        std::vector <VKPipeline*>   pipelines;

    };


    // interface for object factories
    class GameFactory
    {

    public:
        GameFactory();
        virtual ~GameFactory();

        virtual Object3d* buildSky(float height, float width, float *time) = 0;
        virtual Object3d* buildSmallAsteroid()                             = 0;
        virtual Object3d* buildLargeAsteroid()                             = 0;

        static GameFactory* getFactory();

        virtual void draw();

        //  delete all implicit constructors 
        GameFactory(const GameFactory&) = delete;
        GameFactory(GameFactory&&)      = delete;

        GameFactory& operator = (const GameFactory&) = delete;
        GameFactory& operator = (GameFactory&&)      = delete;


    protected:
        Object3d* buildAsteroid(Mesh* mesh, CollisionShape* collision = nullptr);
        std::vector <CollisionShape*> collisions;

        CollisionShape* createLargeAsteroidCollision(float scale = 1.0f);
        CollisionShape* createSmallAsteroidCollision(float scale = 1.0f);


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
        Object3d* buildSmallAsteroid();

        virtual void draw() override;


    private:
        GLInstancedMesh*        asteroid;
        std::vector <GLShader*> shaders;
        GLTexture*              asteroidTex;

        uint32_t                asteroidIndex = 0;

        void buildMeshPrereq();

    };


    // concrete implementation for openGL Graphics, uses instancing for building asteroids
    // built object must not call init
    class OpenGLInstancedGraphicsFactory final : public GameFactory {
    public:
        OpenGLInstancedGraphicsFactory();
        virtual ~OpenGLInstancedGraphicsFactory();

        Object3d* buildSky(float, float, float*);
        Object3d* buildLargeAsteroid();
        Object3d* buildSmallAsteroid();

        virtual void draw() override;


    private:
        GLInstancedMesh*    asteroidInstance[2] = {nullptr, nullptr};
        GLShader*           asteroidShader[2]   = {nullptr, nullptr};
        GLShader*           skyShader;
        GLTexture*          asteroidTex;
        uint32_t            asteroidsLargeIndex = 0;
        uint32_t            asteroidsSmallIndex = 0;

        void buildMeshPrereq();

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
        Object3d* buildSmallAsteroid();

        virtual void draw() override;


    private:
        VKTexture*                asteroidTex;
        std::vector <VKShader*>   shaders;
        std::vector <VKPipeline*> pipelines;

        void buildMeshPrereq();

        Object3d* buildAsteroid(const char* fbxPath, const char* hullPath);

    };


    // concrete implementation for Vulkan Graphics using instancing
    // built object must not call init
    // vulkan-related resources are lazy loaded and freed on delete
    class VulkanInstacedGraphicsFactory final : public GameFactory {
    public:
        VulkanInstacedGraphicsFactory();
        virtual ~VulkanInstacedGraphicsFactory();

        Object3d* buildSky(float, float, float*);
        Object3d* buildLargeAsteroid();
        Object3d* buildSmallAsteroid();

        virtual void draw() override;


    private:
        VKTexture*       asteroidTex;

        VKPipeline*      meshPipeline[2]         = {nullptr, nullptr};
        VKInstancedMesh* asteroidInstance[2]     = {nullptr, nullptr};
        VKVertexShader*  meshVertexShader[2]     = {nullptr, nullptr};
        VKShader*        meshFragmentShader[2]   = {nullptr, nullptr};
                         
        VKShader*        skyFragmentShader;
        VKVertexShader*  skyVertexShader;
                         
        VKPipeline*      skyPipeline;

        uint32_t         asteroidsLargeCount = 0;
        uint32_t         asteroidsSmallCount = 0;

        void buildSkyPrereq();
        void buildAsteroidPrereq();
        void buildMeshPrereq(VKPipeline*&, VKInstancedMesh*&, VKVertexShader*&, VKShader*&, const char*);

    };

};
