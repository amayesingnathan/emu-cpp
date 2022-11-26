#pragma once

#include "Core/Common.h"

namespace Emu {

    enum class FramebufferTextureFormat
    {
        None = 0,

        RGBA8, RED_INTEGER,

        DEPTH24STENCIL8,

        Depth = DEPTH24STENCIL8
    };

    struct FramebufferAttachmentSpec
    {
        std::vector<FramebufferTextureFormat> attachments;

        FramebufferAttachmentSpec() = default;
        FramebufferAttachmentSpec(std::initializer_list<FramebufferTextureFormat> list) : attachments(list) {}
    };

    struct FramebufferSpec
    {
        int width, height;
        FramebufferAttachmentSpec attachments;
        uint samples = 1;

        bool swapChainTarget = true;
    };

    class Framebuffer
    {
    public:
        Framebuffer(const FramebufferSpec& spec);
        ~Framebuffer();

        void bind();
        void unbind();

        void blit(int screenWidth, int screenHeight);

        void resize(usize width, usize height);

        void clearAttachment(uint attachmentIndex, int value);
        void bindColourAttachment(uint index = 0);
        uint getColourAttachment(uint index = 0) const { return mColourAttachments[index]; }

        const FramebufferSpec& getSpec() const { return mSpecification; }

        static Ref<Framebuffer> Create(const FramebufferSpec& spec) { return std::make_shared<Framebuffer>(spec); }

    private:
        void Invalidate();

    private:
        uint mRendererID = 0;
        FramebufferSpec mSpecification;

        std::vector<FramebufferTextureFormat> mColourAttachmentSpecs;
        FramebufferTextureFormat mDepthAttachmentSpec = FramebufferTextureFormat::None;

        std::vector<uint> mColourAttachments;
        uint mDepthAttachment;

    };
}