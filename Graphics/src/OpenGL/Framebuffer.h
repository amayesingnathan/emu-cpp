#pragma once

#include "Core/Common.h"

namespace GL {

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
        Int32 width, height;
        FramebufferAttachmentSpec attachments;
        UInt32 samples = 1;

        bool swapChainTarget = true;
    };

    class Framebuffer
    {
    public:
        Framebuffer(const FramebufferSpec& spec);
        ~Framebuffer();

        void bind();
        void unbind();

        void blit(Int32 screenWidth, Int32 screenHeight);

        void resize(USize width, USize height);

        void clearAttachment(UInt32 attachmentIndex, Int32 value);
        void bindColourAttachment(UInt32 index = 0);
        UInt32 getColourAttachment(UInt32 index = 0) const { return mColourAttachments[index]; }

        const FramebufferSpec& getSpec() const { return mSpecification; }

    private:
        void Invalidate();

    private:
        UInt32 mRendererID = 0;
        FramebufferSpec mSpecification;

        std::vector<FramebufferTextureFormat> mColourAttachmentSpecs;
        FramebufferTextureFormat mDepthAttachmentSpec = FramebufferTextureFormat::None;

        std::vector<UInt32> mColourAttachments;
        UInt32 mDepthAttachment;

    };
}